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
export class realm_handle;

// module_handle's full definition lives in "module_handle.h" (included in the
// module purview of the implementation units module.cc / agent.cc /
// native_module.cc / realm.cc). Defining it HERE, in this interface partition,
// makes clang serialize its visible-name lookup table into this partition's
// BMI, which crashes clang 23's ASTWriter::GenerateNameLookupTable /
// getLookupVisibility (infinite redecl-chain recursion, llvm #161215) when
// cross-compiling for Android. No other interface partition names module_handle
// (agent.h.cc / realm.h.cc only need the option forward-declarations below), so
// it does not need to be an exported entity of this partition.

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

// remote_module_link_record's full definition lives in "module_options.h"
// (included in the implementation units) to keep its std::vector<shared_remote>
// specialization out of this interface partition's BMI.
struct remote_module_link_record;

// subscriber_capability's full definition lives in "subscriber_capability.h"
// (included only in module.cc). It holds util::lockable<move_only_function<...>>
// whose std::unique_ptr specialization crashes clang 22's ASTWriter if
// serialized into this interface partition's BMI.
class subscriber_capability;

// module_handle's full definition lives in "module_handle.h" (see above).

// subscriber_capability's full definition (with its util::lockable<
// move_only_function<...>> member, whose std::unique_ptr specialization crashes
// clang 22's ASTWriter when serialized into this interface BMI) lives in
// "subscriber_capability.h", #included in module.cc's module purview. Only a
// forward declaration is needed here (see above).

} // namespace backend_napi_v8
