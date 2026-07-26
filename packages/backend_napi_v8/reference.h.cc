export module backend_napi_v8:reference;
import :agent_handle;
import auto_js;
import napi_js;
import v8_js;

namespace backend_napi_v8 {

// environment is only used by-reference in the signatures below, so a forward
// declaration suffices. We deliberately do NOT `import :environment` here:
// importing it (on top of :agent_handle + napi_js) merges a large visible
// name-lookup table that clang 22 crashes on while serializing this partition's
// BMI (ASTWriter::GenerateNameLookupTable / getLookupVisibility, llvm #161215).
// forward_promise_type is the alias from :environment expanded inline.
class environment;
using forward_promise_type = js::forward<js::napi::local_of<js::promise_tag>>;

export class reference_handle {
	public:
		using transfer_type = js::tagged_external<reference_handle>;

		explicit reference_handle(js::null_tag /*tag*/);
		explicit reference_handle(js::undefined_tag /*tag*/);
		reference_handle(agent_handle agent, js::typeof_kind type_of, js::iv8::shared_remote<v8::Context> realm, js::iv8::shared_remote<v8::Value> value);
		reference_handle(const agent_handle::lock& lock, agent_handle agent, js::iv8::shared_remote<v8::Context> realm, v8::Local<v8::Value> value);
		reference_handle(const agent_handle::lock& lock, agent_handle agent, js::iv8::shared_remote<v8::Context> realm, v8::Local<v8::Object> value);
		auto copy(environment& env) -> forward_promise_type;
		auto get(environment& env, js::string_t name) -> forward_promise_type;
		auto set(environment& env, js::string_t name, js::forward<js::napi::local_of<>> value_local) -> forward_promise_type;
		auto invoke(environment& env, js::forward<js::napi::local_of<js::list_tag>> params_local) -> forward_promise_type;

		static auto class_template(environment& env) -> js::napi::local_of<js::class_tag_of<reference_handle>>;

	private:
		agent_handle agent_;
		js::iv8::shared_remote<v8::Context> realm_;
		js::iv8::shared_remote<v8::Value> value_;
		js::typeof_kind typeof_;
};

} // namespace backend_napi_v8
