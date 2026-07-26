module;
#include "auto_js/gcc_abi_tag.h"
export module backend_napi_v8:agent;
import :agent_handle;
import :module_;
import :script;
import auto_js;
import napi_js;
import std;

namespace backend_napi_v8 {

// agent_handle_value's full definition (its nested clock_* / create_options
// structs and the `namespace js` transfer_type / union_of specializations) lives
// in "agent_handle_value.h" (included in the module purview of the
// implementation units agent.cc / main.cc). Defining it HERE, in this interface
// partition, makes clang serialize its struct_template specializations and
// visible-name lookup table into this partition's BMI, which crashes clang 23
// (struct_template instantiation / ASTWriter::GenerateNameLookupTable, llvm
// #161215 family) when cross-compiling for Android. No other interface partition
// names agent_handle_value, so it need not be an exported entity here.
class agent_handle_value;

} // namespace backend_napi_v8
