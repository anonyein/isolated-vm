// Internal option-struct definitions for the module_ partition.
//
// These structs carry `std::optional<js::iv8::source_origin>` and
// `js::struct_template` reflection members. Defining them inside the
// `backend_napi_v8:module_` interface partition forces clang 22 to
// instantiate std::optional<source_origin> / std::tuple<struct_template
// components> and serialize those specializations into the partition's BMI,
// which crashes clang 22's ASTWriter (llvm #165348 family). The interface
// partition therefore only forward-declares these types; their full
// definitions live here and are #included in the *module purview* of the
// implementation units (module.cc / agent.cc / realm.cc) that actually need
// complete types. Implementation units emit no BMI, so no serialization crash.
//
// This header must be included AFTER the unit's `import` declarations so that
// js::, js::iv8::, and util:: names are visible; the definitions then attach
// to the backend_napi_v8 module purview.
#pragma once

namespace backend_napi_v8 {

// module_handle's full definition lives in "module_handle.h" (included after
// this header in the implementation units). module_handle_link_record only
// stores js::tagged_external<module_handle>, which does not need a complete
// type, so a forward declaration suffices here.
class module_handle;

struct compile_module_options : js::optional_constructible {
		using js::optional_constructible::optional_constructible;
		std::optional<js::iv8::source_origin> origin;

		constexpr static auto struct_template = js::struct_template{
			js::struct_member{util::cw<"origin">, &compile_module_options::origin},
		};
};

struct create_capability_options {
		std::u16string origin;

		constexpr static auto struct_template = js::struct_template{
			js::struct_member{util::cw<"origin">, &create_capability_options::origin},
		};
};

struct module_handle_link_record {
		std::vector<js::tagged_external<module_handle>> modules;
		std::vector<unsigned> payload;

		constexpr static auto struct_template = js::struct_template{
			js::struct_member{util::cw<"modules">, &module_handle_link_record::modules},
			js::struct_member{util::cw<"payload">, &module_handle_link_record::payload},
		};
};

// Purely internal (never in an interface signature); carries
// std::vector<shared_remote<v8::Module>> whose specialization crashes clang 22's
// ASTWriter if serialized into the :module_ interface BMI.
struct remote_module_link_record {
		std::vector<js::iv8::shared_remote<v8::Module>> modules;
		std::vector<unsigned> payload;
};

} // namespace backend_napi_v8
