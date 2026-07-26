#pragma once
// reference_handle was the `backend_napi_v8:reference` interface partition. It
// is NOT re-exported by the primary module (`_module.cc` does not list it) and
// is only consumed by two implementation units (realm.cc / reference.cc). We
// converted it from a module interface partition to a plain header so it never
// emits a BMI: clang 23 crashes in ASTWriter::GenerateNameLookupTable /
// getLookupVisibility (infinite redecl-chain recursion, llvm #161215) while
// serializing this partition's visible-name lookup table when cross-compiling
// for an Android target. Including it in the module purview of the .cc units
// (after their imports) gives it the same visibility without a BMI.
//
// Requires the including TU to have already imported: auto_js, napi_js, v8_js,
// :agent_handle, :environment.

namespace backend_napi_v8 {

class reference_handle {
	public:
		using transfer_type = js::tagged_external<reference_handle>;

		explicit reference_handle(js::null_tag /*tag*/);
		explicit reference_handle(js::undefined_tag /*tag*/);
		reference_handle(agent_handle agent, js::typeof_kind type_of, js::iv8::shared_remote<v8::Context> realm, js::iv8::shared_remote<v8::Value> value);
		reference_handle(const agent_handle::lock& lock, agent_handle agent, js::iv8::shared_remote<v8::Context> realm, v8::Local<v8::Value> value);
		reference_handle(const agent_handle::lock& lock, agent_handle agent, js::iv8::shared_remote<v8::Context> realm, v8::Local<v8::Object> value);
		auto copy(environment& env) -> forward_promise_type;
		auto get(environment& env, js::string_t name) -> forward_promise_type;
		auto set(environment& env, js::string_t name, js::forward<js::napi::local_of<>> value_local) -> forward_promise_type;
		auto invoke(environment& env, js::forward<js::napi::local_of<js::list_tag>> params_local) -> forward_promise_type;

		static auto class_template(environment& env) -> js::napi::local_of<js::class_tag_of<reference_handle>>;

	private:
		agent_handle agent_;
		js::iv8::shared_remote<v8::Context> realm_;
		js::iv8::shared_remote<v8::Value> value_;
		js::typeof_kind typeof_;
};

} // namespace backend_napi_v8
