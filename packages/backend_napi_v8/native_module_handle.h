#pragma once
// create_native_module_options / native_module_handle were defined in the
// `backend_napi_v8:native_module` interface partition. Defining them there makes
// clang serialize the struct_template specialization + native_module_handle's
// visible-name lookup table into that partition's BMI, which crashes clang 23
// (struct_template instantiation / ASTWriter::GenerateNameLookupTable, llvm
// #161215 family) when cross-compiling for Android. Moved to a plain header,
// included in the module purview of the implementation units, so no BMI is
// emitted for them.
//
// Requires the including TU to have already imported: auto_js, isolated_vm,
// napi_js, nodejs, util, :realm, :environment, :agent_handle.

namespace backend_napi_v8 {

struct create_native_module_options {
		std::u16string origin;
		std::optional<std::string> suffix;

		constexpr static auto struct_template = js::struct_template{
			js::struct_member{util::cw<"origin">, &create_native_module_options::origin},
			js::struct_member{util::cw<"suffix">, &create_native_module_options::suffix},
		};
};

class native_module_handle {
	public:
		explicit native_module_handle(
			js::napi::uv_dlib lib,
			isolated_vm::detail::initialize_addon* initialize,
			create_native_module_options options,
			std::vector<std::u16string> names
		);

		auto instantiate(environment& env, realm_handle* realm) -> forward_promise_type;
		static auto class_template(environment& env) -> js::napi::local_of<js::class_tag_of<native_module_handle>>;
		static auto create(environment& env, std::string filename, create_native_module_options options) -> forward_promise_type;
		static auto unload_hook() -> void;

	private:
		js::napi::uv_dlib lib_;
		isolated_vm::detail::initialize_addon* initialize_;
		create_native_module_options options_;
		std::vector<std::u16string> names_;
};

} // namespace backend_napi_v8
