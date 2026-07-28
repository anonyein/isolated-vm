#include "environment.h"
#include "executor.h"
#include "node_wrapper.h"
#include "scheduler.h"
#include <memory>
#include <v8.h>
#include <utility>

using namespace v8;
namespace ivm {
namespace {
	thread_pool_t thread_pool{std::thread::hardware_concurrency() + 1};
}

/*
 * Scheduler implementation
 */
void Scheduler::CancelAsync() {
	if (async_wait != nullptr) {
		async_wait->Cancel();
	}
}

void Scheduler::DoneRunning() {
	assert(status == Status::Running);
	status = Status::Waiting;
}

void Scheduler::InterruptIsolate() {
	assert(status == Status::Running);
	// Since this callback will be called by v8 we can be certain the pointer to `isolate` is still valid
	env.GetIsolate()->RequestInterrupt([](Isolate* /*isolate_ptr*/, void* env_ptr) {
		static_cast<IsolateEnvironment*>(env_ptr)->InterruptEntryAsync();
	}, &env);
}

void Scheduler::InterruptSyncIsolate() {
	env.GetIsolate()->RequestInterrupt([](Isolate* /*isolate_ptr*/, void* env_ptr) {
		static_cast<IsolateEnvironment*>(env_ptr)->InterruptEntrySync();
	}, &env);
}

auto Scheduler::WakeIsolate(std::shared_ptr<IsolateEnvironment> isolate_ptr) -> bool {
	if (status == Status::Waiting) {
		status = Status::Running;
		// Move shared reference to this scheduler to ensure the IsolateEnvironment won't be deleted
		// before a thread picks up this work.
		assert(!env_ref);
		env_ref = std::move(isolate_ptr);
		IncrementUvRef();
		SendWake();
		return true;
	} else {
		return false;
	}
}

auto LockedScheduler::GetForegroundTaskRunner() -> std::shared_ptr<v8::TaskRunner> {
	return env.GetTaskRunner();
}

void LockedScheduler::DecrementUvRefForIsolate(const std::shared_ptr<IsolateHolder>& holder) {
	auto ref = holder->GetIsolate();
	if (ref) {
		ref->scheduler->DecrementUvRef();
	}
}

void LockedScheduler::IncrementUvRefForIsolate(const std::shared_ptr<IsolateHolder>& holder) {
	auto ref = holder->GetIsolate();
	if (ref) {
		ref->scheduler->IncrementUvRef();
	}
}

IsolatedScheduler::IsolatedScheduler(IsolateEnvironment& env, UvScheduler& default_scheduler) :
	LockedScheduler{env},
	default_scheduler{default_scheduler} {}

void IsolatedScheduler::DecrementUvRef() {
	default_scheduler.DecrementUvRef();
}

void IsolatedScheduler::IncrementUvRef() {
	default_scheduler.IncrementUvRef();
}

void IsolatedScheduler::SendWake() {
	thread_pool.exec(thread_affinity, [](bool pool_thread, void* param) {
		auto& scheduler = *static_cast<IsolatedScheduler*>(param);
		auto ref = std::exchange(scheduler.env_ref, {});
		ref->AsyncEntry();
		if (!pool_thread) {
			ref->GetIsolate()->DiscardThreadSpecificMetadata();
		}
		// Grab reference to default scheduler, since resetting `ref` may deallocate `scheduler` and
		// invalidate the instance. Resetting `ref` must take place here because the destructor might
		// invoke cleanup tasks on the default isolate which will increment `uv_ref_count`.
		// `uv_ref_count` needs to be incremented before it's decremented otherwise `UvScheduler` will
		// try to invoke `uv_ref` from a non-default thread.
		auto& default_scheduler = scheduler.default_scheduler;
		ref = {};
		if (--default_scheduler.uv_ref_count == 0) {
			// Wake up the libuv loop so we can unref the async handle from the default thread.
			uv_async_send(default_scheduler.uv_async);
		}
	}, this);
}

UvScheduler::UvScheduler(IsolateEnvironment& env) :
		LockedScheduler{env},
		loop{node::GetCurrentEventLoop(v8::Isolate::GetCurrent())},
		uv_async{new uv_async_t} {
	uv_async_init(loop, uv_async, [](uv_async_t* async) {
		auto& scheduler = *static_cast<UvScheduler*>(async->data);
		auto ref = [&]() {
			// Lock is required to access env_ref on the default scheduler but a non-default scheduler
			// doesn't need it. This is because `WakeIsolate` can trigger this function via
			// `uv_async_send` while another thread is writing to `env_ref`
			std::lock_guard<std::mutex> lock{scheduler.mutex};
			return std::exchange(scheduler.env_ref, {});
		}();
		if (ref) {
			ref->AsyncEntry();
			--scheduler.uv_ref_count;
		}
		// This callback is the sole authority over the handle's ref state, and always runs on the
		// loop thread (under a host embedder like Javet it runs inside `uv_run`, which is wrapped in
		// this isolate's v8::Locker, so it is mutually exclusive with any locker-holding thread that
		// touches the handle directly in `Increment`/`DecrementUvRef`). Reconcile the handle so it is
		// reffed exactly when there is outstanding work. This both unrefs completed work and, crucially,
		// applies any `uv_ref` that `IncrementUvRef` deferred from a non-locker thread (e.g. an ivm
		// worker scheduling Phase3 back to node) -- without this the loop could report `!uv_loop_alive`
		// and the embedder's `await` would return before the pending work ran, dropping the result.
		// `uv_ref`/`uv_unref` are idempotent so reconciling unconditionally is safe.
		if (scheduler.uv_ref_count.load() > 0) {
			uv_ref(reinterpret_cast<uv_handle_t*>(scheduler.uv_async));
		} else {
			uv_unref(reinterpret_cast<uv_handle_t*>(scheduler.uv_async));
		}
	});
	uv_async->data = this;
	uv_unref(reinterpret_cast<uv_handle_t*>(uv_async));
}

UvScheduler::~UvScheduler() {
	uv_close(reinterpret_cast<uv_handle_t*>(uv_async), [](uv_handle_t* handle) {
		delete reinterpret_cast<uv_async_t*>(handle);
	});
}

void UvScheduler::DecrementUvRef() {
	if (--uv_ref_count == 0) {
		if (Executor::IsDefaultThread()) {
			uv_unref(reinterpret_cast<uv_handle_t*>(uv_async));
		} else {
			uv_async_send(uv_async);
		}
	}
}

void UvScheduler::IncrementUvRef() {
	if (++uv_ref_count == 1) {
		// Upstream asserts `IsDefaultThread()` here because it assumes a single fixed loop thread.
		// A host embedder like Javet drives the same node loop from a rotating thread pool, so that
		// assertion does not hold. When we hold the isolate's locker we are mutually exclusive with
		// `uv_run` and can touch the handle directly; otherwise defer the wake to the loop.
		if (v8::Locker::IsLocked(env.GetIsolate())) {
			uv_ref(reinterpret_cast<uv_handle_t*>(uv_async));
		} else {
			uv_async_send(uv_async);
		}
	}
}

void UvScheduler::SendWake() {
	uv_async_send(uv_async);
}

/*
 * Scheduler::AsyncWait implementation
 */
Scheduler::AsyncWait::~AsyncWait() {
	scheduler.Lock()->async_wait = nullptr;
}

void Scheduler::AsyncWait::Cancel() {
	*state.write() = canceled;
	state.notify_one();
}

void Scheduler::AsyncWait::Done() {
	*state.write() = finished;
	state.notify_one();
}

auto Scheduler::AsyncWait::Wait() const -> Scheduler::AsyncWait::State {
	auto lock = state.read<true>();
	while (*lock == pending) {
		lock.wait();
	}
	return *lock;
}

} // namespace ivm
