// Internal subscriber_capability type for the backend_napi_v8 module.
//
// subscriber_capability holds a `util::lockable<util::move_only_function<...>>`
// member. Defining it inside the `backend_napi_v8:module_` interface partition
// forces clang 22 to instantiate the std::unique_ptr<move_only_function_virtual
// <...>> specialization and serialize it into the partition's BMI, which
// crashes clang 22's ASTWriter (llvm #165348 family).
//
// It is only ever used inside module.cc (make / class_template / send /
// create_capability), never named in any interface-partition signature, so its
// full definition lives here and is #included only in module.cc's module
// purview. Implementation units emit no BMI, so nothing is serialized.
//
// Must be #included AFTER module.cc's `import` declarations (auto_js, napi_js,
// util, std, v8_js, :environment) so the referenced names are visible.
#pragma once

namespace backend_napi_v8 {

class subscriber_capability {
	private:
		struct private_constructor {
				explicit private_constructor() = default;
		};

	public:
		using callback_type = util::move_only_function<auto(js::value_t) const->bool>;
		using transfer_type = js::tagged_external<subscriber_capability>;
		class subscriber;

		explicit subscriber_capability(private_constructor /*private*/) {};
		auto accept_callback(callback_type callback) -> void;
		auto take_subscriber() -> std::shared_ptr<subscriber>;
		auto send(environment& env, js::forward<napi::local_of<>> message_local) -> bool;
		static auto make(environment& env) -> js::napi::local_of<js::object_tag>;

		static auto class_template(environment& env) -> js::napi::local_of<js::class_tag_of<subscriber_capability>>;

	private:
		util::lockable<callback_type> callback_;
		std::shared_ptr<subscriber> subscriber_;
};

class subscriber_capability::subscriber {
	public:
		explicit subscriber(const std::shared_ptr<subscriber_capability>& capability);
		auto subscribe(callback_type callback) -> void;

	private:
		std::weak_ptr<subscriber_capability> capability_;
		bool subscribed_ = false;
};

} // namespace backend_napi_v8
