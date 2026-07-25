export module napi_js:class_;
import :object;
import std;

namespace js::napi {

template <class Type>
class local_for_class_of : public local_next<class_tag_of<Type>> {
	public:
		// Construct a new C++ instance & JavaScript value
		template <class... Args>
		auto construct(auto& env, Args&&... args) const -> local_of<object_tag>
			requires std::constructible_from<Type, Args...>;

		template <class... HostArgs, class... RuntimeArgs>
		auto runtime_construct(auto& env, std::tuple<HostArgs...> host_args, std::tuple<RuntimeArgs...> runtime_args) const -> local_of<object_tag>
			requires std::constructible_from<Type, HostArgs...>;

		// Create a JavaScript value from an already-created instance smart pointer
		template <class... Args>
		auto transfer_construct(auto& env, auto instance, std::tuple<Args...> runtime_args) const -> local_of<object_tag>;

		template <class Environment>
		static auto make(Environment& env, const auto& class_template) -> local_of<class_tag_of<Type>>;
};

} // namespace js::napi
