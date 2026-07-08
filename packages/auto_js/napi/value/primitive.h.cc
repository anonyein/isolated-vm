export module napi_js:primitive;
import :api;
import :environment;
import :handle.local_of;
import :handle.value_of;
import std;

namespace js::napi {

// boolean
class value_for_boolean : public value_next<boolean_tag> {
	public:
		using value_next<boolean_tag>::value_next;
		[[nodiscard]] explicit operator bool() const;
};

class value_for_false : public value_next<false_tag> {
	public:
		using value_next<false_tag>::value_next;
		[[nodiscard]] constexpr explicit operator bool() const { return false; }
};

class value_for_true : public value_next<true_tag> {
	public:
		using value_next<true_tag>::value_next;
		[[nodiscard]] constexpr explicit operator bool() const { return true; }
};

// number
class value_for_number : public value_next<number_tag> {
	public:
		using value_next<number_tag>::value_next;
		[[nodiscard]] explicit operator double() const;
		[[nodiscard]] explicit operator std::int32_t() const;
};

// bigint
class value_for_bigint : public value_next<bigint_tag> {
	public:
		using value_next<bigint_tag>::value_next;
		[[nodiscard]] explicit operator bigint() const;
		[[nodiscard]] explicit operator std::int64_t() const;
};

// string
class value_for_string : public value_next<string_tag> {
	public:
		using value_next<string_tag>::value_next;
		[[nodiscard]] explicit operator std::string() const;
		[[nodiscard]] explicit operator std::u16string() const;
};

} // namespace js::napi
