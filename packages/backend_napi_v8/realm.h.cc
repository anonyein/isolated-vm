export module backend_napi_v8:realm;
import :agent_handle;
import :environment;
import :module_;
import auto_js;
import napi_js;
import v8_js;

namespace backend_napi_v8 {

export class realm_handle {
	public:
		using transfer_type = js::tagged_external<realm_handle>;

		realm_handle(agent_handle agent, js::iv8::shared_remote<v8::Context> realm);

		auto agent() -> agent_handle& { return agent_; }
		auto realm() -> js::iv8::shared_remote<v8::Context>& { return realm_; }

		auto acquire_global_object(environment& env) -> forward_promise_type;
		auto instantiate_runtime(environment& env) -> forward_promise_type;
		auto create_capability(environment& env, forward_callback_type make_capability, create_capability_options options) -> forward_promise_type;

		static auto create(environment& env, agent_handle& agent) -> forward_promise_type;
		static auto class_template(environment& env) -> js::napi::local_of<js::class_tag_of<realm_handle>>;

	private:
		agent_handle agent_;
		js::iv8::shared_remote<v8::Context> realm_;
};

} // namespace backend_napi_v8
