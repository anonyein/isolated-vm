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
};

struct create_capability_options {
		std::u16string origin;
};

struct module_handle_link_record {
		std::vector<js::tagged_external<module_handle>> modules;
		std::vector<unsigned> payload;
};

// Purely internal (never in an interface signature); carries
// std::vector<shared_remote<v8::Module>> whose specialization crashes clang 22's
// ASTWriter if serialized into the :module_ interface BMI.
struct remote_module_link_record {
		std::vector<js::iv8::shared_remote<v8::Module>> modules;
		std::vector<unsigned> payload;
};

} // namespace backend_napi_v8

// nb: The struct reflection is defined as an EXTERNAL js::struct_properties
// specialization here (after the complete class definitions) rather than as an
// in-class `constexpr static auto struct_template` member. Evaluating that
// consteval member during the parse of the struct body (complete-class context)
// with a pointer-to-member of a std::optional<source_origin> field crashes
// clang here when cross-compiling for Android. Defining the reflection after the
// class avoids that in-body consteval evaluation. See auto_js/js/struct/types.cc
// for the struct_properties primary template.
namespace js {

template <>
struct struct_properties<backend_napi_v8::compile_module_options> {
		constexpr static auto defaultable = std::is_nothrow_constructible_v<backend_napi_v8::compile_module_options, std::nullopt_t>;
		constexpr static auto properties = js::struct_template{
			js::struct_member{util::cw<"origin">, &backend_napi_v8::compile_module_options::origin},
		};
};

template <>
struct struct_properties<backend_napi_v8::create_capability_options> {
		constexpr static auto defaultable = std::is_nothrow_constructible_v<backend_napi_v8::create_capability_options, std::nullopt_t>;
		constexpr static auto properties = js::struct_template{
			js::struct_member{util::cw<"origin">, &backend_napi_v8::create_capability_options::origin},
		};
};

template <>
struct struct_properties<backend_napi_v8::module_handle_link_record> {
		constexpr static auto defaultable = std::is_nothrow_constructible_v<backend_napi_v8::module_handle_link_record, std::nullopt_t>;
		constexpr static auto properties = js::struct_template{
			js::struct_member{util::cw<"modules">, &backend_napi_v8::module_handle_link_record::modules},
			js::struct_member{util::cw<"payload">, &backend_napi_v8::module_handle_link_record::payload},
		};
};

} // namespace js
