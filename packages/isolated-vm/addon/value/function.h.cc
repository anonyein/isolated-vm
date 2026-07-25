module;
#include "auto_js/export_tag.h"
export module isolated_vm:value.function;
import :handle.local_of;
import :support.lock_fwd;
import :value.primitive;

namespace isolated_vm {
using namespace js;

// local_of<function_tag>
class EXPORT local_for_function : public local_next<function_tag> {
	public:
		using local_next<function_tag>::local_next;

		template <class Result = std::monostate>
		auto apply(const runtime_lock& lock, auto&& args) -> Result;

		template <class Result = std::monostate>
		auto call(const runtime_lock& lock, auto&&... args) -> Result;

	private:
		auto invoke(const runtime_lock& lock, local_of<> that, std::span<local_of<>> argv) -> local_of<>;
};

} // namespace isolated_vm
