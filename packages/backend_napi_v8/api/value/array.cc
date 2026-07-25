module isolated_vm;
import :support.cast;
import :support.lock;
import :value.array;
import v8_js;

namespace isolated_vm {
using namespace js;

// local_for_array
auto local_for_array::set(const runtime_lock& lock, int key, local_of<> value) const -> void {
	iv8::unmaybe(cast_in(*this)->Set(unwrap_lock_witness(lock).context(), static_cast<std::uint32_t>(key), cast_in(value)));
}

auto local_for_array::make(const runtime_lock& lock, int capacity) -> local_of<list_tag> {
	return local_of<list_tag>::from(cast_out(v8::Array::New(unwrap_lock_witness(lock).isolate(), capacity)));
}

} // namespace isolated_vm
