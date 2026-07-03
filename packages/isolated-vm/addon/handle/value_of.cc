export module isolated_vm:handle.value_of;
import :handle.types;
import :support.lock_fwd;

namespace isolated_vm {

// Member & method implementation for stateful objects. Used internally in visitors.
export template <class Tag>
class value_of : public value_specialization<Tag>::bound_type {
	public:
		using value_specialization<Tag>::bound_type::bound_type;
};

template <class Tag>
value_of(auto, local_of<Tag>) -> value_of<Tag>;

template <>
class value_of<void> : public value_handle {
	protected:
		value_of() = default;
		value_of(const runtime_lock& lock, value_handle value) :
				value_handle{value},
				lock_{&lock} {}

		[[nodiscard]] auto lock() const -> const runtime_lock& { return *lock_; }

	private:
		const runtime_lock* lock_{};
};

// Details applied to each level of the `value_of<T>` hierarchy.
template <class Tag>
class value_next : public value_of<typename Tag::tag_type> {
	public:
		using tag_type = Tag;
		using value_of<typename Tag::tag_type>::value_of;
		value_next() = default;
		value_next(const runtime_lock& lock, local_of<Tag> value) :
				value_of<typename Tag::tag_type>{lock, value_handle{value}} {}

		// NOLINTNEXTLINE(google-explicit-constructor)
		operator local_of<Tag>() const { return local_of<Tag>::from(value_handle{*this}); }
};

} // namespace isolated_vm
