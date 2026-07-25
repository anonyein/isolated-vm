module;
#include "auto_js/export_tag.h"
export module isolated_vm:value.primitive;
import :handle.local_of;
import :handle.value_of;
import auto_js;
import std;

namespace isolated_vm {
using namespace js;

// Reminder: Don't sort these
export enum class value_typeof : std::uint8_t {
	// primitives
	undefined,
	null,
	boolean,
	number,
	number_i32,
	string,
	string_latin1,
	symbol,
	bigint,
	bigint_i64,

	// objects
	function,
	date,
	array,
	dictionary,
	external,
	promise,
	array_buffer,
	shared_array_buffer,

	// typed arrays
	bigint64_array,
	biguint64_array,
	data_view,
	float16_array,
	float32_array,
	float64_array,
	int16_array,
	int32_array,
	int8_array,
	uint16_array,
	uint32_array,
	uint8_array,
	uint8_clamped_array,
};

// value & primitive
class EXPORT local_for_value : public local_next<value_tag> {
	public:
		using local_next<value_tag>::local_next;
		[[nodiscard]] auto is_array() const -> bool;
		[[nodiscard]] auto is_bigint() const -> bool;
		[[nodiscard]] auto is_boolean() const -> bool;
		[[nodiscard]] auto is_date() const -> bool;
		[[nodiscard]] auto is_function() const -> bool;
		[[nodiscard]] auto is_null() const -> bool;
		[[nodiscard]] auto is_number() const -> bool;
		[[nodiscard]] auto is_string() const -> bool;
		[[nodiscard]] auto is_undefined() const -> bool;
		[[nodiscard]] auto inspect() const -> value_typeof;
};

class EXPORT local_for_primitive : public local_next<primitive_tag> {
	public:
		using local_next<primitive_tag>::local_next;
		[[nodiscard]] auto inspect() const -> value_typeof;
};

// null & undefined
class EXPORT local_for_undefined : public local_next<undefined_tag> {
	public:
		using local_next<undefined_tag>::local_next;
		static auto make(const basic_lock& lock) -> local_of<undefined_tag>;
};

class EXPORT local_for_null : public local_next<null_tag> {
	public:
		using local_next<null_tag>::local_next;
		static auto make(const basic_lock& lock) -> local_of<null_tag>;
};

// boolean
class EXPORT local_for_boolean : public local_next<boolean_tag> {
	public:
		using local_next<boolean_tag>::local_next;
		static auto make(const basic_lock& lock, bool value) -> local_of<boolean_tag>;
};

class EXPORT value_for_boolean : public value_next<boolean_tag> {
	public:
		using value_next<boolean_tag>::value_next;
		[[nodiscard]] explicit operator bool() const;
};

// number
class EXPORT local_for_number : public local_next<number_tag> {
	public:
		using local_next<number_tag>::local_next;
		[[nodiscard]] auto inspect() const -> value_typeof;
		static auto make(const basic_lock& lock, double value) -> local_of<number_tag_of<double>>;
		static auto make(const basic_lock& lock, std::int32_t value) -> local_of<number_tag_of<std::int32_t>>;
};

class EXPORT value_for_number : public value_next<number_tag> {
	public:
		using value_next<number_tag>::value_next;
		[[nodiscard]] explicit operator double() const;
		[[nodiscard]] explicit operator std::int32_t() const;
};

// name
class EXPORT local_for_name : public local_next<name_tag> {
	public:
		using local_next<name_tag>::local_next;
		[[nodiscard]] auto inspect() const -> value_typeof;
};

// string
class EXPORT local_for_string : public local_next<string_tag> {
	public:
		using local_next<string_tag>::local_next;
		[[nodiscard]] auto inspect() const -> value_typeof;
		[[nodiscard]] auto is_latin1() const -> bool;
		static auto make(const basic_lock& lock, std::u16string_view value) -> local_of<string_tag_of<char16_t>>;
		static auto make(const basic_lock& lock, std::string_view value) -> local_of<string_tag_of<char>>;
};

class EXPORT value_for_string : public value_next<string_tag> {
	public:
		using value_next<string_tag>::value_next;
		[[nodiscard]] explicit operator std::string() const;
		[[nodiscard]] explicit operator std::u16string() const;
};

// bigint
class EXPORT local_for_bigint : public local_next<bigint_tag> {
	public:
		using local_next<bigint_tag>::local_next;
		[[nodiscard]] auto inspect() const -> value_typeof;
		static auto make(const runtime_lock& lock, js::bigint value) -> local_of<bigint_tag_of<js::bigint>>;
		static auto make(const basic_lock& lock, std::int64_t value) -> local_of<bigint_tag_of<std::int64_t>>;
};

class EXPORT value_for_bigint : public value_next<bigint_tag> {
	public:
		using value_next<bigint_tag>::value_next;
		[[nodiscard]] explicit operator js::bigint() const;
		[[nodiscard]] explicit operator std::int64_t() const;
};

} // namespace isolated_vm
