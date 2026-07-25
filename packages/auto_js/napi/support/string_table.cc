export module napi_js:string_table;
import :reference;
import auto_js;
import std;
import util;
using namespace std::string_literals;

namespace js::napi {

// nb: The lookup table stores a *trivial* value type (std::monostate) rather than
// napi::reference<>. Storing reference<> directly in the sealed_map used to make clang
// (20/21/22) segfault in the frontend (getCanonicalTemplateArgument /
// LoadExternalSpecializations) while serializing `std::array<std::pair<string_view,
// reference<>>, N>` into this module's BMI. Decoupling the compile-time lookup (keys ->
// sorted slot) from the runtime reference storage (a parallel std::array indexed by that
// slot) keeps reference<> out of any sealed_map/pair specialization, dodging the ICE while
// preserving identical behavior.
template <const auto& Strings>
constexpr auto string_index_of = []() -> auto {
	const auto [... strings ] = Strings;
	return util::sealed_map{std::type_identity<std::monostate>{}, strings...};
}();

// You can turn off `strict` if you don't care about a string table
struct string_table_options {
		bool strict = true;
};

// Environment storage for string literals
export template <const auto& Strings, string_table_options Options = {}>
class string_table {
	public:
		auto string_table_storage(auto string_value) {
			constexpr auto string_sv = util::make_consteval_string_view(string_value);
			constexpr auto index = string_index_of<Strings>.lookup(string_sv);
			if constexpr (index) {
				return util::just<napi::reference<string_tag>&>{string_literal_storage_.at(*index)};
			} else {
				// static_assert(!Options.strict, std::format("String literal '{}' is missing in storage", string_sv));
				static_assert(!Options.strict, "String literal '"s + string_sv + "' is missing in storage"s);
				return util::nothing<napi::reference<string_tag>&>{};
			}
		}

	private:
		static constexpr std::size_t table_size_ = std::tuple_size_v<std::remove_cvref_t<decltype(Strings)>>;
		std::array<napi::reference<string_tag>, table_size_> string_literal_storage_;
};

} // namespace js::napi
