module;
#include "auto_js/gcc_abi_tag.h"
export module v8_js:evaluation.origin;
import auto_js;
import std;
import util;

namespace js::iv8 {

export struct source_location {
		int line{};
		int column{};

		constexpr static auto struct_template = js::struct_template{
			js::struct_member{util::cw<"line">, &source_location::line},
			js::struct_member{util::cw<"column">, &source_location::column},
		};
};

export struct source_origin {
		std::optional<std::u16string> name;
		std::optional<source_location> location;

		GCC_ABI_TAG constexpr static auto struct_template = js::struct_template{
			js::struct_member{util::cw<"name">, &source_origin::name},
			js::struct_member{util::cw<"location">, &source_origin::location},
		};
};

} // namespace js::iv8

// nb: Force std::optional<source_origin>'s specialization (and the
// is_trivially_move_constructible<source_origin> trait query it performs at
// optional:802) to be instantiated HERE, in the module that defines
// source_origin, so it is baked into v8_js's BMI. Importers (backend_napi_v8)
// then reuse it instead of re-computing the trait across the module boundary,
// which crashes clang 22/23 when cross-compiling for Android (llvm #161215
// family: trait / name-lookup computation on an imported type). Explicit
// instantiation must be at a scope that encloses namespace std.
template class std::optional<js::iv8::source_origin>;
