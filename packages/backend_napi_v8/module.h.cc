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

struct remote_module_link_record {
		std::vector<js::iv8::shared_remote<v8::Module>> modules;
		std::vector<unsigned> payload;
};

class subscriber_capability;

export class module_handle {
	public:
		using transfer_type = js::tagged_external<module_handle>;
		module_handle(agent_handle agent, js::iv8::shared_remote<v8::Module> module);

		auto agent() -> auto& { return agent_; }

		auto evaluate(environment& env, realm_handle* realm) -> forward_promise_type;
		auto link(environment& env, realm_handle* realm, module_handle_link_record link_record) -> forward_promise_type;
		static auto class_template(environment& env) -> js::napi::local_of<class_tag_of<module_handle>>;
		static auto compile(environment& env, agent_handle& agent, js::string_t source_text, compile_module_options options) -> forward_promise_type;
		static auto create_capability(environment& env, realm_handle& realm, js::napi::local_of<js::function_tag> make_capability, create_capability_options options) -> forward_promise_type;

	private:
		agent_handle agent_;
		js::iv8::shared_remote<v8::Module> module_;
};

class subscriber_capability {
	private:
		struct private_constructor {
				explicit private_constructor() = default;
		};

	public:
		using callback_type = util::move_only_function<auto(js::value_t) const->bool>;
		using transfer_type = js::tagged_external<subscriber_capability>;
		class subscriber;

		explicit subscriber_capability(private_constructor /*private*/) {};
		auto accept_callback(callback_type callback) -> void;
		auto take_subscriber() -> std::shared_ptr<subscriber>;
		auto send(environment& env, js::forward<napi::local_of<>> message_local) -> bool;
		static auto make(environment& env) -> js::napi::local_of<js::object_tag>;

		static auto class_template(environment& env) -> js::napi::local_of<js::class_tag_of<subscriber_capability>>;

	private:
		util::lockable<callback_type> callback_;
		std::shared_ptr<subscriber> subscriber_;
};

class subscriber_capability::subscriber {
	public:
		explicit subscriber(const std::shared_ptr<subscriber_capability>& capability);
		auto subscribe(callback_type callback) -> void;

	private:
		std::weak_ptr<subscriber_capability> capability_;
		bool subscribed_ = false;
};

} // namespace backend_napi_v8
