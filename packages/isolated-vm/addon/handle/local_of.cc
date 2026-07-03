export module isolated_vm:handle.local_of;
export import :handle.types;
import auto_js;
import std;

namespace isolated_vm {
using namespace js;

// Details applied to each level of the `local_of<T>` hierarchy.
template <class Tag>
class local_next : public local_of<typename Tag::tag_type> {
	public:
		using local_of<typename Tag::tag_type>::local_of;

		// "Downcast" to a more specific tag. Potentially unsafe.
		template <std::convertible_to<Tag> To>
		auto cast(To /*tag*/) const -> local_of<To> { return local_of<To>::from(*this); }

		// Construct from any `value_handle`. Potentially unsafe.
		static auto from(value_handle value_) -> local_of<Tag> { return std::bit_cast<local_of<Tag>>(value_); }
};

// Tagged isolated_vm value
export template <class Tag>
class local_of : public value_specialization<Tag>::value_type {
	public:
		using value_specialization<Tag>::value_type::value_type;
};

template <std::derived_from<value_of<void>> Type>
local_of(Type) -> local_of<typename Type::tag_type>;

// Sentinel instantiation
template <>
class local_of<void> : public value_handle {
	public:
		using value_handle::value_handle;
};

} // namespace isolated_vm

// Specialize for `js::forward`. Makes `js::forward{local_of<Tag>}` infer
// `js::forward<Tag, ...>`.
namespace js {
template <class Tag>
struct forward_tag_for<isolated_vm::local_of<Tag>> : std::type_identity<Tag> {};
} // namespace js
