export module isolated_vm:handle.types;
import auto_js;

namespace isolated_vm {
using namespace js;

// `runtime_handle` is the base class of `local_of<T>` and `value_of<T>` member types.
class runtime_handle {
	public:
		explicit runtime_handle(void* value) : value_{value} {}
		runtime_handle() = default;

		// Check empty value
		explicit operator bool() const { return value_ != nullptr; }

	private:
		void* value_{};
};

// Forward declarations for handle types which declare template deduction guides on each other.
export template <class Tag = value_tag>
class local_of;

export template <class Tag = value_tag>
class value_of;

} // namespace isolated_vm
