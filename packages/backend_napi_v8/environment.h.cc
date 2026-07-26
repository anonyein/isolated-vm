export module backend_napi_v8:environment;
import auto_js;
import napi_js;
import std;
import v8_js;
// NOTE: deliberately NO `using namespace js;` here. This partition is imported
// by every other backend interface partition (module_, reference, ...). A
// namespace-scope using-directive that pulls the entire (huge) js:: namespace
// inflates this module's visible name-lookup table, which clang 22 serializes
// via ASTWriter::GenerateNameLookupTable -- and that routine SmallVector-
// overflows / segfaults once the merged table (this partition + napi_js + v8_js
// + agent_handle) grows large enough (the module.h.cc / reference.h.cc crashes).
// Fully-qualifying the handful of js:: names we use keeps the table small.
using namespace std::string_view_literals;

namespace backend_napi_v8 {

// String literals used in this module.
// nb: A C-style array (NOT std::array/std::tuple). The 25-element
// std::array<string_view,25> is a class-template specialization that clang's
// ASTReader aborts on (FoldingSetVector<ClassTemplateSpecializationDecl> ->
// realloc) while deserializing it from the std module BMI. A builtin array is
// not a class-template specialization, so it never touches that code path. The
// consumers (napi::string_table / class_template_references) and the reference
// NTTP `template <const auto& Strings>` bind a C array identically, and
// `const auto [...strings] = Strings;` works the same on a C array.
constexpr std::string_view string_literals[] = {
	"Agent"sv,
	"attributes"sv,
	"clock"sv,
	"column"sv,
	"complete"sv,
	"copy"sv,
	"epoch"sv,
	"error"sv,
	"get"sv,
	"initialize"sv,
	"interval"sv,
	"line"sv,
	"location"sv,
	"memoryLimitBytes"sv,
	"Module"sv,
	"modules"sv,
	"name"sv,
	"origin"sv,
	"payload"sv,
	"randomSeed"sv,
	"result"sv,
	"specifier"sv,
	"suffix"sv,
	"timeout"sv,
	"type"sv,
};

// Storage for class templates (C-style array; see string_literals above).
constexpr std::string_view class_names[] = {
	"Agent"sv,
	"Module"sv,
	"NativeModule"sv,
	"Realm"sv,
	"Reference"sv,
	"Script"sv,
	"SubscriberCapability"sv,
};

// Instance of the `isolated-vm` module, once per nodejs environment.
export class environment
		: public js::napi::environment,
			public js::napi::string_table<string_literals>,
			public js::napi::class_template_references<class_names> {
	public:
		explicit environment(napi_env env);
		~environment();

		auto agent_class() -> js::napi::local_of<js::function_tag> { return agent_class_.get(*this); }
		auto cluster() -> js::iv8::isolated::cluster& { return cluster_; }
		// NOLINTNEXTLINE(performance-unnecessary-value-param)
		auto destroy_orphan_scheduler(std::any isolate_scheduler) -> void;
		auto module_class() -> js::napi::local_of<js::function_tag> { return module_class_.get(*this); }

		auto make_initialize() -> js::napi::local_of<js::function_tag>;

	private:
		js::iv8::isolated::cluster cluster_;
		js::napi::reference<js::function_tag> agent_class_;
		js::napi::reference<js::function_tag> module_class_;
};

// Common types
using forward_callback_type = js::forward<js::napi::local_of<js::function_tag>>;
using forward_promise_type = js::forward<js::napi::local_of<js::promise_tag>>;

} // namespace backend_napi_v8
