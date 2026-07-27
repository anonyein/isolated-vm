// Consumer TU that imports module `m` and forces the crash pattern:
// a type with std::optional<imported type> members whose implicit special
// members get declared while an ADL-found constrained operator is checked,
// pulling decls out of m's BMI.
module;
#include <optional>
#include <string>
export module consumer;
import m;

namespace consumer {

// An imported type used through std::optional (matches source_origin usage).
struct origin {
	std::optional<std::u16string> name;
	std::optional<int> line;
};

// Option struct: std::optional<imported-ish type> + a reflection bundle of it,
// mirroring compile_module_options + struct_template.
export struct options {
	std::optional<origin> value;
	repro::bundle<origin, int> reflect;
};

// Force implicit special-member declaration + ADL/constraint checking by
// value-constructing and comparing.
export auto make() -> options {
	options a{std::optional<origin>{}, repro::bundle<origin, int>{origin{}, 0}};
	options b{a};        // implicit copy ctor -> walks imported BMI
	(void)b;
	return a;
}

} // namespace consumer
