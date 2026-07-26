module;
#include <napi_js_initialize.h>
// main.cc is an implementation unit of `backend_napi_v8` (it is a PRIVATE source
// of the target). As an implementation unit it implicitly imports the primary
// module interface, so all exported entities (environment, agent_handle_value,
// realm_handle, script_handle, native_module_handle, ...) are visible without an
// explicit `import backend_napi_v8;`. This also lets us #include "module_handle.h"
// in the module purview so module_handle attaches to `backend_napi_v8` with the
// SAME linkage as in the other implementation units (module.cc etc.). Consuming
// it as a non-module TU instead would attach module_handle to the global module
// and mismatch the module-attached member symbols the other units define.
module backend_napi_v8;
import :agent;
import :agent_handle;
import :environment;
import :module_;
import :native_module;
import :realm;
import :script;
import auto_js;
import napi_js;
import std;
import util;
import v8_js;

// module_handle: was the exported class of the :module_ partition, now a plain
// header (moved out to avoid the clang 23 ASTWriter::GenerateNameLookupTable
// crash on Android cross-compile). Included in the module purview here.
#include "module_handle.h"

using namespace backend_napi_v8;

// Sanity check ensuring that, at least in principle, `js::transfer` can directly transfer from
// runtime to runtime without intermediates.
auto check_transfer(
	environment& napi_lock,
	napi_value napi_local,
	js::iv8::context_lock_witness v8_lock,
	v8::Local<v8::Value> v8_local
) -> void {
	[[maybe_unused]] auto yy = js::transfer_out_strict<std::span<const std::uint8_t>>(v8_local.As<v8::Uint8Array>(), v8_lock);
	std::ignore = js::transfer<napi_value>(v8_local, std::forward_as_tuple(v8_lock), std::forward_as_tuple(napi_lock));
	std::ignore = js::transfer<v8::Local<v8::Value>>(napi_local, std::forward_as_tuple(napi_lock), std::forward_as_tuple(v8_lock));
}

// Initialize this module
js::napi::napi_js_module module_namespace{
	std::type_identity<environment>{},
	[](environment& env) -> auto {
		return std::tuple{
			std::in_place,
			std::pair{util::cw<"initialize">, js::forward{env.make_initialize()}},
			std::pair{util::cw<"Agent">, js::forward{agent_handle_value::class_template(env)}},
			std::pair{util::cw<"Module">, js::forward{module_handle::class_template(env)}},
			std::pair{util::cw<"NativeModule">, js::forward{native_module_handle::class_template(env)}},
			std::pair{util::cw<"Realm">, js::forward{realm_handle::class_template(env)}},
			std::pair{util::cw<"Script">, js::forward{script_handle::class_template(env)}},
		};
	}
};
