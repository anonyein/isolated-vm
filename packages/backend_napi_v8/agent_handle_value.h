#pragma once
// agent_handle_value (and its nested clock_* / create_options structs, plus the
// `namespace js` transfer_type / union_of specializations) was the exported
// content of the `backend_napi_v8:agent` interface partition. Defining it there
// makes clang serialize the struct_template specializations and
// agent_handle_value's visible-name lookup table into that partition's BMI,
// which crashes clang 23 (struct_template instantiation /
// ASTWriter::GenerateNameLookupTable, llvm #161215 family) when cross-compiling
// for Android. Moved to a plain header, included in the module purview of the
// implementation units (agent.cc / main.cc), so no BMI is emitted for it.
//
// Requires the including TU to have already imported: auto_js, napi_js, util,
// v8_js, :agent_handle, :environment, :module_, :script.

namespace backend_napi_v8 {

// Value held by napi
class agent_handle_value {
	public:
		struct create_options;
		struct clock_deterministic;
		struct clock_microtask;
		struct clock_realtime;
		struct clock_system;

		explicit agent_handle_value(agent_handle agent) : agent_{std::move(agent)} {}
		auto create_realm(environment& env) -> forward_promise_type;
		auto compile_module(environment& env, js::string_t source_text, compile_module_options options) -> forward_promise_type;
		auto compile_script(environment& env, js::string_t source_text, compile_script_options options) -> forward_promise_type;
		auto dispose_async(environment& env) -> forward_promise_type;
		static auto create(environment& env, std::optional<create_options> options_optional) -> forward_promise_type;
		static auto class_template(environment& env) -> js::napi::local_of<js::class_tag_of<agent_handle_value>>;

	private:
		agent_handle agent_;
		js::napi::reference<js::promise_tag> disposed_;
};

// Clock options
struct agent_handle_value::clock_deterministic {
		js::js_clock::time_point epoch;
		double interval{};

		constexpr static auto struct_template = js::struct_template{
			js::struct_member{util::cw<"epoch">, &clock_deterministic::epoch},
			js::struct_member{util::cw<"interval">, &clock_deterministic::interval},
		};
};

struct agent_handle_value::clock_microtask {
		std::optional<js::js_clock::time_point> epoch;

		constexpr static auto struct_template = js::struct_template{
			js::struct_member{util::cw<"epoch">, &clock_microtask::epoch},
		};
};
struct agent_handle_value::clock_realtime {
		js::js_clock::time_point epoch;

		constexpr static auto struct_template = js::struct_template{
			js::struct_member{util::cw<"epoch">, &clock_realtime::epoch},
		};
};

struct agent_handle_value::clock_system {
		constexpr static auto struct_template = js::struct_template{};
};

// `Agent.create()` options
struct agent_handle_value::create_options {
		using clock_type = std::variant<clock_deterministic, clock_microtask, clock_realtime, clock_system>;

		std::optional<clock_type> clock;
		std::optional<double> memory_limit_bytes;
		std::optional<double> random_seed;

		constexpr static auto struct_template = js::struct_template{
			js::struct_member{util::cw<"clock">, &create_options::clock},
			js::struct_member{util::cw<"memoryLimitBytes">, &create_options::memory_limit_bytes},
			js::struct_member{util::cw<"randomSeed">, &create_options::random_seed},
		};
};

} // namespace backend_napi_v8

// Options visitors & acceptors
namespace js {
using backend_napi_v8::agent_handle_value;

template <>
struct transfer_type<agent_handle_value> : std::type_identity<js::tagged_external<agent_handle_value>> {};

template <>
struct union_of<agent_handle_value::create_options::clock_type> {
		constexpr static auto& discriminant = util::cw<"type">;
		// util::flat_tuple (not std::tuple): instantiating std::tuple<alternative...>
		// crashes clang 23 when cross-compiling for Android (llvm #161215 family).
		constexpr static auto alternatives = util::flat_tuple{
			alternative<agent_handle_value::clock_deterministic>{"deterministic"},
			alternative<agent_handle_value::clock_microtask>{"microtask"},
			alternative<agent_handle_value::clock_realtime>{"realtime"},
			alternative<agent_handle_value::clock_system>{"system"},
		};
};

} // namespace js
