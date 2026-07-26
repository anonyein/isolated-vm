module;
#include "auto_js/gcc_abi_tag.h"
export module backend_napi_v8:realm;
import :agent_handle;
import :environment;
import :module_;
import auto_js;
import napi_js;
import v8_js;

namespace backend_napi_v8 {

// realm_handle's full definition lives in "realm_handle.h" (included in the
// module purview of the implementation units). Defining it HERE, in this
// interface partition, makes clang serialize its visible-name lookup table into
// this partition's BMI, which crashes clang 23's
// ASTWriter::GenerateNameLookupTable / getLookupVisibility (infinite
// redecl-chain recursion, llvm #161215) when cross-compiling for Android. Other
// partitions that mention realm_handle (native_module.h.cc / script.h.cc) only
// use it by pointer, so this forward declaration is sufficient for them.
export class realm_handle;

} // namespace backend_napi_v8
