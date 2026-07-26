module;
#include "auto_js/gcc_abi_tag.h"
export module backend_napi_v8:script;
import :realm;
import auto_js;
import napi_js;
import std;
import util;

namespace backend_napi_v8 {

// compile_script_options / run_script_options / script_handle's full definitions
// live in "script_handle.h" (included in the module purview of the
// implementation units script.cc / agent.cc / main.cc). Defining them HERE, in
// this interface partition, makes clang serialize their
// std::optional<source_origin> / struct_template specializations and
// script_handle's visible-name lookup table into this partition's BMI, which
// crashes clang 23 (struct_template instantiation / ASTWriter, llvm #161215
// family) when cross-compiling for Android. The only other partition that names
// these (agent.h.cc) uses them by value in *declarations* (incomplete types are
// fine there), so forward declarations suffice in the interface.
export struct compile_script_options;
export struct run_script_options;
export class script_handle;

} // namespace backend_napi_v8
