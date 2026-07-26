module;
#include "auto_js/gcc_abi_tag.h"
export module backend_napi_v8:module_;
import :agent_handle;
import :environment;
import napi_js;
import std;
import util;
import v8_js;

namespace backend_napi_v8 {
export class module_handle;
export class realm_handle;

// NOTE: The full definitions of these option structs live in
// "module_options.h" and are #included in the module purview of the
// implementation units (module.cc / agent.cc / realm.cc). They carry
// std::optional<js::iv8::source_origin> and js::struct_template reflection
// members; defining them HERE (in this interface partition) forces clang 22
// to serialize those std::optional / std::tuple specializations into this
// partition's BMI, which crashes clang 22's ASTWriter (llvm #165348 family).
// Only by-value parameter *declarations* below need these names, and a
// by-value parameter in a declaration accepts an incomplete type, so a
// forward declaration is sufficient in the interface.
struct compile_module_options;
struct create_capability_options;
struct module_handle_link_record;

// remote_module_link_record's full definition lives in "module_options.h"
// (included in the implementation units) to keep its std::vector<shared_remote>
// specialization out of this interface partition's BMI.
struct remote_module_link_record;

// subscriber_capability's full definition lives in "subscriber_capability.h"
// (included only in module.cc). It holds util::lockable<move_only_function<...>>
// whose std::unique_ptr specialization crashes clang 22's ASTWriter if
// serialized into this interface partition's BMI.
class subscriber_capability;

export class module_handle {
	public:
		using transfer_type = js::tagged_external<module_handle>;
		module_handle(agent_handle agent, js::iv8::shared_remote<v8::Module> module);

		auto agent() -> agent_handle& { return agent_; }

		auto evaluate(environment& env, realm_handle* realm) -> forward_promise_type;
		auto link(environment& env, realm_handle* realm, module_handle_link_record link_record) -> forward_promise_type;
		static auto class_template(environment& env) -> js::napi::local_of<js::class_tag_of<module_handle>>;
		static auto compile(environment& env, agent_handle& agent, js::string_t source_text, compile_module_options options) -> forward_promise_type;
		static auto create_capability(environment& env, realm_handle& realm, js::napi::local_of<js::function_tag> make_capability, create_capability_options options) -> forward_promise_type;

	private:
		agent_handle agent_;
		js::iv8::shared_remote<v8::Module> module_;
};

// subscriber_capability's full definition (with its util::lockable<
// move_only_function<...>> member, whose std::unique_ptr specialization crashes
// clang 22's ASTWriter when serialized into this interface BMI) lives in
// "subscriber_capability.h", #included in module.cc's module purview. Only a
// forward declaration is needed here (see above).

} // namespace backend_napi_v8
