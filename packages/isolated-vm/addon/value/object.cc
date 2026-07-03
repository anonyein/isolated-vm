module;
#include "auto_js/export_tag.h"
export module isolated_vm:value.object;
import :value.primitive;

namespace isolated_vm {
using namespace js;

// local_of<object_tag>
class EXPORT local_for_object : public local_next<object_tag> {
	public:
		using local_next<object_tag>::local_next;
		[[nodiscard]] auto inspect() const -> value_typeof;
};

} // namespace isolated_vm
