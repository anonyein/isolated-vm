#pragma once
// compile_script_options / run_script_options / script_handle were defined in
// the `backend_napi_v8:script` interface partition. Defining them there makes
// clang serialize their std::optional<source_origin> / struct_template
// specializations (and script_handle's visible-name lookup table) into that
// partition's BMI, which crashes clang 23 (ASTWriter::GenerateNameLookupTable /
// struct_template instantiation, llvm #161215 family) when cross-compiling for
// Android. The only other interface partition that mentions these is agent.h.cc,
// which takes compile_script_options only by value in a *declaration* (an
// incomplete type is fine there) -- so :script keeps just forward declarations
// and the full definitions live here, #included in the module purview of the
// implementation units (script.cc / agent.cc / main.cc).
//
// Requires the including TU to have already imported: auto_js, napi_js, util,
// v8_js, std, :agent_handle, :environment, :realm.

namespace backend_napi_v8 {

struct compile_script_options : js::optional_constructible {
		using js::optional_constructible::optional_constructible;
		std::optional<js::iv8::source_origin> origin;

		constexpr static auto struct_template = js::struct_template{
			js::struct_member{util::cw<"origin">, &compile_script_options::origin},
		};
};

struct run_script_options : js::optional_constructible {
		using js::optional_constructible::optional_constructible;
		std::optional<double> timeout;

		constexpr static auto struct_template = js::struct_template{
			js::struct_member{util::cw<"timeout">, &run_script_options::timeout},
		};
};

class script_handle {
	public:
		using transfer_type = js::tagged_external<script_handle>;
		using script_type = js::iv8::shared_remote<v8::UnboundScript>;

		explicit script_handle(script_type script) :
				script_{std::move(script)} {}

		auto run(environment& env, realm_handle* realm, run_script_options options) -> forward_promise_type;

		static auto class_template(environment& env) -> js::napi::local_of<js::class_tag_of<script_handle>>;
		static auto compile_script(environment& env, agent_handle& agent, js::string_t code_string, compile_script_options options) -> forward_promise_type;

	private:
		script_type script_;
};

} // namespace backend_napi_v8
