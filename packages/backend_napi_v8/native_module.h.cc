module;
#include "auto_js/gcc_abi_tag.h"
export module backend_napi_v8:native_module;
import :realm;
import auto_js;
import isolated_vm;
import napi_js;
import nodejs;
import std;
import util;

namespace backend_napi_v8 {

// create_native_module_options and native_module_handle's full definitions live
// in "native_module_handle.h" (included in the module purview of the
// implementation units native_module.cc / main.cc / environment.cc). Defining
// them HERE, in this interface partition, makes clang serialize the
// struct_template specialization and native_module_handle's visible-name lookup
// table into this partition's BMI, which crashes clang 23 (struct_template
// instantiation / ASTWriter::GenerateNameLookupTable, llvm #161215 family) when
// cross-compiling for Android. No other interface partition names these, so they
// need not be exported entities of this partition.
struct create_native_module_options;
class native_module_handle;

} // namespace backend_napi_v8
