export module napi_js:handle.types;
import auto_js;
import nodejs;

namespace js::napi {

// `runtime_handle` is the base class of `local_of<T>` and `value_of<T>` member types.
class runtime_handle {
	protected:
	public:
		explicit runtime_handle(int, napi_value value) : value_{value} {}
		explicit runtime_handle(napi_value value) : value_{value} {}
		runtime_handle() = default;

		// Implicit cast back to a `napi_value`
		// NOLINTNEXTLINE(google-explicit-constructor)
		operator napi_value() const { return value_; }

		// Check empty value
		explicit operator bool() const { return value_ != nullptr; }

	private:
		napi_value value_{};
};

// Forward declarations for handle types which declare template deduction guides on each other.
export template <class Tag = value_tag>
class local_of;

export template <class Tag = value_tag>
class value_of;

} // namespace js::napi
