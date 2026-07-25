module;
#include "auto_js/export_tag.h"
export module isolated_vm:value.array;
import :handle.local_of;
import :value.record;
import auto_js;

namespace isolated_vm {
using namespace js;

// local_of<list_tag>
class EXPORT local_for_array : public local_next<list_tag> {
	public:
		using local_next<list_tag>::local_next;
		auto set(const runtime_lock& lock, int key, local_of<> value) const -> void;
		static auto make(const runtime_lock& lock, int capacity = 0) -> local_of<list_tag>;
};

} // namespace isolated_vm
