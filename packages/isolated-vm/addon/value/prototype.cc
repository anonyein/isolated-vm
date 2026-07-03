module;
#include "auto_js/export_tag.h"
export module isolated_vm:value.prototype;
import :handle.local_of;
import :support.callback_storage;

namespace isolated_vm {
using namespace js;

// local_of<prototype_tag>
class EXPORT local_for_prototype : public local_next<prototype_tag> {
	public:
		using local_next<prototype_tag>::local_next;
		[[nodiscard]] static auto make(const basic_lock& lock, runtime_callback_data_allocated_type data, int length) -> local_of<function_prototype_tag>;
		[[nodiscard]] static auto make(const basic_lock& lock, runtime_callback_data_span_type data, int length) -> local_of<function_prototype_tag>;

		template <class Type>
		[[nodiscard]] static auto make(const basic_lock& lock, runtime_callback_function_storage<Type> data, int length) -> local_of<function_prototype_tag> {
			auto data_span = std::span{reinterpret_cast<std::byte*>(&data), sizeof(data)};
			return make(lock, data_span, length);
		}
};

} // namespace isolated_vm
