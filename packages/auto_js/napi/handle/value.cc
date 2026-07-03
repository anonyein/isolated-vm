export module napi_js:value_of;
import :handle.types;
import nodejs;

namespace js::napi {

// Details applied to each level of the `value_of<T>` hierarchy.
template <class Tag>
class value_next : public value_of<typename Tag::tag_type> {
	public:
		using tag_type = Tag;
		using value_of<typename Tag::tag_type>::value_of;
		value_next() = default;
		value_next(napi_env env, local_of<Tag> value) :
				value_of<typename Tag::tag_type>{env, napi_value{value}} {}

		// NOLINTNEXTLINE(google-explicit-constructor)
		operator local_of<Tag>() const { return local_of<Tag>::from(napi_value{*this}); }
};

// Member & method implementation for stateful objects. Used internally in visitors. I think it
// might make sense to have the environment specified by a template parameter. Then you would use
// `value_of<T>` or something instead of passing the environment to each `local_of<T>` method.
template <class Tag>
class value_of : public value_specialization<Tag>::value_type {
	public:
		using value_specialization<Tag>::value_type::value_type;
};

template <class Tag>
value_of(auto, local_of<Tag>) -> value_of<Tag>;

template <>
class value_of<void> : public value_handle {
	protected:
		value_of() = default;
		value_of(napi_env env, napi_value value) :
				value_handle{value},
				env_{env} {}

		[[nodiscard]] auto env() const -> napi_env { return env_; }

	private:
		napi_env env_{};
};

} // namespace js::napi
