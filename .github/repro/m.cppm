// Minimal module `m`: exports a class template that is stored/looked-up in a
// way that forces clang to serialize a FunctionTemplateDecl + a constrained
// ADL operator into the BMI. Mirrors util::flat_tuple / struct_template +
// cw_operators (the constrained friend operator== found via ADL).
module;
#include <cstddef>
#include <utility>
export module m;

namespace repro {

// A constrained friend operator== reachable only by ADL (mirrors
// util::constant_wrapper's cw_operators::operator==). Checking it during
// implicit special-member declaration is what walks the imported BMI.
template <class T>
struct wrapper {
	T value;
	template <class U>
		requires requires(T a, U b) { a == b; }
	friend constexpr auto operator==(wrapper /*l*/, wrapper<U> /*r*/) -> bool { return true; }
};

// Class template stored via private inheritance + friend get (mirrors
// flat_tuple). Its specializations are what get lazily loaded from the BMI.
template <std::size_t I, class T>
struct elem { T v; };

template <class Seq, class... Ts>
struct storage;

template <std::size_t... I, class... Ts>
struct storage<std::index_sequence<I...>, Ts...> : private elem<I, Ts>... {
	constexpr explicit storage(Ts... xs) : elem<I, Ts>{std::move(xs)}... {}
};

export template <class... Ts>
struct bundle : storage<std::index_sequence_for<Ts...>, Ts...> {
	using storage<std::index_sequence_for<Ts...>, Ts...>::storage;
};

} // namespace repro
