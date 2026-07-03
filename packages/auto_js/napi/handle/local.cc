export module napi_js:value_handle;
import :handle.types;
import auto_js;
import nodejs;
import std;

namespace js::napi {

// Heirarchy:
// local_of<object_tag> ->
// local_for_object ->
// local_next<object_tag> ->
// local_of<value_tag> -> ...

// Details applied to each level of the `local_of<T>` hierarchy.
template <class Tag>
class local_next : public local_of<typename Tag::tag_type> {
	public:
		using local_of<typename Tag::tag_type>::local_of;

		// "Downcast" to a more specific tag. Potentially unsafe.
		template <std::convertible_to<Tag> To>
		auto cast(To /*tag*/) const -> local_of<To> { return local_of<To>::from(*this); }

		// Construct from any `napi_value`. Potentially unsafe.
		static auto from(napi_value value_) -> local_of<Tag> { return std::bit_cast<local_of<Tag>>(value_); }
};

// Tagged napi_value
export template <class Tag>
class local_of : public value_specialization<Tag>::local_type {
	public:
		using value_specialization<Tag>::local_type::local_type;
};

// Deduction guide for `local_of{value_of}`
template <class Type>
	requires requires { typename Type::tag_type; }
local_of(Type value) -> local_of<typename Type::tag_type>;

// Sentinel instantiation
template <>
class local_of<void> : public value_handle {
	public:
		using value_handle::value_handle;
};

} // namespace js::napi

// Specialize for `js::forward`. Makes `js::forward{local_of<Tag>}` infer
// `js::forward<Tag, ...>`.
namespace js {
template <class Tag>
struct forward_tag_for<napi::local_of<Tag>> : std::type_identity<Tag> {};
} // namespace js
