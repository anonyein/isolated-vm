// Consumer that imports module m and forces the crash path:
//  - defines option structs holding bundle<...> reflection + wrapper<...>
//  - takes them by value / constructs them, forcing clang to declare implicit
//    special members, check the ADL-reachable constrained operator==, and lazily
//    load the imported template specializations from m's BMI.
import m;

#include <optional>
#include <string>
#include <vector>

namespace app {

struct opt_a {
	std::optional<std::u16string> origin;
	// reflection member mirroring `constexpr static auto struct_template = ...`
	static constexpr auto refl = repro::bundle<repro::wrapper<int>>{repro::wrapper<int>{1}};
};

struct opt_b {
	std::u16string origin;
	static constexpr auto refl = repro::bundle<repro::wrapper<int>>{repro::wrapper<int>{2}};
};

struct opt_c {
	std::vector<int> modules;
	std::vector<unsigned> payload;
	static constexpr auto refl =
		repro::bundle<repro::wrapper<int>, repro::wrapper<int>>{repro::wrapper<int>{3}, repro::wrapper<int>{4}};
};

// By-value params -> implicit copy ctor declaration + constraint check + ADL.
auto take_a(opt_a a) -> std::size_t { return sizeof(a); }
auto take_b(opt_b b) -> std::size_t { return sizeof(b); }
auto take_c(opt_c c) -> std::size_t { return sizeof(c); }

} // namespace app

auto main() -> int {
	return static_cast<int>(app::take_a({}) + app::take_b({}) + app::take_c({}));
}
