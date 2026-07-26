#pragma once
// realm_handle was the exported class of the `backend_napi_v8:realm` interface
// partition. Other interface partitions that mention it (native_module.h.cc /
// script.h.cc) only use it by pointer, so a forward declaration suffices there.
// We moved it from the module interface partition to a plain header so the
// `:realm` partition no longer emits a BMI containing realm_handle's
// visible-name lookup table: clang 23 crashes in
// ASTWriter::GenerateNameLookupTable / getLookupVisibility (infinite
// redecl-chain recursion, llvm #161215) while serializing that table when
// cross-compiling for an Android target. Including this header in the module
// purview of the consuming units (after their imports) gives realm_handle the
// same visibility without ever emitting a BMI for it.
//
// Requires the including TU to have already imported: auto_js, napi_js, v8_js,
// :agent_handle, :environment.

namespace backend_napi_v8 {

// create_capability_options is fully defined in "module_options.h" (included in
// the implementation units). A by-value parameter in a *declaration* accepts an
// incomplete type, so a forward declaration is sufficient here.
struct create_capability_options;

class realm_handle {
	public:
		using transfer_type = js::tagged_external<realm_handle>;

		realm_handle(agent_handle agent, js::iv8::shared_remote<v8::Context> realm);

		auto agent() -> agent_handle& { return agent_; }
		auto realm() -> js::iv8::shared_remote<v8::Context>& { return realm_; }

		auto acquire_global_object(environment& env) -> forward_promise_type;
		auto instantiate_runtime(environment& env) -> forward_promise_type;
		auto create_capability(environment& env, forward_callback_type make_capability, create_capability_options options) -> forward_promise_type;

		static auto create(environment& env, agent_handle& agent) -> forward_promise_type;
		static auto class_template(environment& env) -> js::napi::local_of<js::class_tag_of<realm_handle>>;

	private:
		agent_handle agent_;
		js::iv8::shared_remote<v8::Context> realm_;
};

} // namespace backend_napi_v8
