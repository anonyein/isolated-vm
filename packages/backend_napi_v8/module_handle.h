#pragma once
// module_handle was the exported class of the `backend_napi_v8:module_`
// interface partition. It is only used by implementation units (module.cc /
// agent.cc / native_module.cc / realm.cc) and the addon entrypoint (main.cc);
// no other interface partition (.h.cc) names it. We moved it from the module
// interface partition to a plain header so the `:module_` partition no longer
// emits a BMI containing module_handle's visible-name lookup table: clang 23
// crashes in ASTWriter::GenerateNameLookupTable / getLookupVisibility (infinite
// redecl-chain recursion, llvm #161215) while serializing that table when
// cross-compiling for an Android target. Including this header in the module
// purview of the consuming units (after their imports) gives module_handle the
// same visibility without ever emitting a BMI for it.
//
// Requires the including TU to have already imported: auto_js, napi_js, util,
// v8_js, :agent_handle, :environment.

namespace backend_napi_v8 {

// Forward declarations. realm_handle is defined in the :realm partition; the
// option structs are fully defined in "module_options.h" (included in the
// implementation units). By-value parameters in *declarations* accept
// incomplete types, and realm_handle is only used by pointer/reference here.
class realm_handle;
struct compile_module_options;
struct create_capability_options;
struct module_handle_link_record;

class module_handle {
	public:
		using transfer_type = js::tagged_external<module_handle>;
		module_handle(agent_handle agent, js::iv8::shared_remote<v8::Module> module);

		auto agent() -> agent_handle& { return agent_; }

		auto evaluate(environment& env, realm_handle* realm) -> forward_promise_type;
		auto link(environment& env, realm_handle* realm, module_handle_link_record link_record) -> forward_promise_type;
		static auto class_template(environment& env) -> js::napi::local_of<js::class_tag_of<module_handle>>;
		static auto compile(environment& env, agent_handle& agent, js::string_t source_text, compile_module_options options) -> forward_promise_type;
		static auto create_capability(environment& env, realm_handle& realm, js::napi::local_of<js::function_tag> make_capability, create_capability_options options) -> forward_promise_type;

	private:
		agent_handle agent_;
		js::iv8::shared_remote<v8::Module> module_;
};

} // namespace backend_napi_v8
