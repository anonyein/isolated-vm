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

		// nb: These special members are explicitly user-declared (though
		// =default) to force clang to treat source_origin as non-trivially
		// move/copy constructible WITHOUT computing it via libc++'s
		// is_trivially_move_constructible<source_origin> trait. clang 22/23
		// segfault (exit 133) while instantiating that trait for this type when
		// cross-compiling for Android (llvm #161215 family); std::optional<T>
		// queries it at optional:802 to pick its storage strategy. A user-declared
		// (non-trivial) move ctor short-circuits that query.
		source_origin() = default;
		source_origin(const source_origin&) = default;
		source_origin(source_origin&&) noexcept;
		auto operator=(const source_origin&) -> source_origin& = default;
		auto operator=(source_origin&&) noexcept -> source_origin&;
		~source_origin() = default;

		GCC_ABI_TAG constexpr static auto struct_template = js::struct_template{
			js::struct_member{util::cw<"name">, &source_origin::name},
			js::struct_member{util::cw<"location">, &source_origin::location},
		};
};

// Out-of-line so the (non-trivial, user-declared) move members don't force the
// trivial-trait computation at the class definition site.
inline source_origin::source_origin(source_origin&&) noexcept = default;
inline auto source_origin::operator=(source_origin&&) noexcept -> source_origin& = default;

} // namespace js::iv8
