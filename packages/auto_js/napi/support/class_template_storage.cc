export module napi_js:class_template_storage;
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
constexpr auto class_template_index_of = []() -> auto {
	const auto [... strings ] = Strings;
	return util::sealed_map{std::type_identity<std::monostate>{}, strings...};
}();

// Environment storage for class template references
export template <const auto& Strings>
class class_template_references {
	public:
		template <class Type>
		auto class_template(this auto& self, std::type_identity<Type> /*type*/, const auto& class_template) -> local_of<class_tag_of<Type>> {
			constexpr auto name_sv = util::make_consteval_string_view(class_template.constructor.name);
			constexpr auto index = class_template_index_of<Strings>.lookup(name_sv);
			if constexpr (!index) {
				// nb: This fails while linking against STL for some reason. So the `constexpr` if works
				// around the issue.
				// D:\a\isolated-vm\isolated-vm\packages\auto_js\napi\support\class_template_storage.cc:38:45:
				//    error: invalid operands to binary expression ('string' (aka 'basic_string<char,
				//    char_traits<char>, allocator<char>>') and 'const std::basic_string_view<char>')
				//    38 |                         static_assert(index, "Class template '"s + name_sv + "' is missing in storage"s);
				static_assert(index, "Class template '"s + name_sv + "' is missing in storage"s);
			}
			auto& reference = self.class_template_references_.at(*index);
			using value_type = js::napi::local_of<class_tag_of<Type>>;
			if (reference) {
				return value_type::from(reference.get(self));
			} else {
				auto template_value = value_type::make(self, class_template);
				reference.reset(self, template_value);
				return template_value;
			}
		}

	private:
		static constexpr std::size_t table_size_ = std::extent_v<std::remove_cvref_t<decltype(Strings)>>;
		std::array<napi::reference<class_tag>, table_size_> class_template_references_;
};

} // namespace js::napi
