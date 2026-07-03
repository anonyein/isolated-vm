module isolated_vm;
import :support.cast;
import :support.lock;
import :value;
import auto_js;

namespace isolated_vm {
using namespace js;

// value & primitive

auto local_for_value::is_array() const -> bool {
	return cast_in(*this)->IsArray();
}

auto local_for_value::is_bigint() const -> bool {
	return cast_in(*this)->IsBigInt();
}

auto local_for_value::is_boolean() const -> bool {
	return cast_in(*this)->IsBoolean();
}

auto local_for_value::is_date() const -> bool {
	return cast_in(*this)->IsDate();
}

auto local_for_value::is_function() const -> bool {
	return cast_in(*this)->IsFunction();
}

auto local_for_value::is_null() const -> bool {
	return cast_in(*this)->IsNull();
}

auto local_for_value::is_number() const -> bool {
	return cast_in(*this)->IsNumber();
}

auto local_for_value::is_string() const -> bool {
	return cast_in(*this)->IsString();
}

auto local_for_value::is_undefined() const -> bool {
	return cast_in(*this)->IsUndefined();
}

auto local_for_value::inspect() const -> value_typeof {
	auto subject = cast_in(*this);
	if (subject->IsPrimitive()) {
		return local_of<primitive_tag>::from(*this).inspect();
	} else {
		return local_of<object_tag>::from(*this).inspect();
	}
}

auto local_for_primitive::inspect() const -> value_typeof {
	auto subject = cast_in(*this);
	if (subject->IsUndefined()) {
		return value_typeof::undefined;
	} else if (subject->IsNull()) {
		return value_typeof::null;
	} else if (subject->IsNumber()) {
		return local_of<number_tag>::from(*this).inspect();
	} else if (subject->IsName()) {
		return local_of<name_tag>::from(*this).inspect();
	} else if (subject->IsBoolean()) {
		return value_typeof::boolean;
	} else if (subject->IsBigInt()) {
		return local_of<bigint_tag>::from(*this).inspect();
	} else {
		std::unreachable();
	}
}

// null & undefined
auto local_for_undefined::make(const basic_lock& lock) -> local_of<undefined_tag> {
	return cast_out(js::transfer_in<v8::Local<iv8::Undefined>>(std::monostate{}, unwrap_lock_witness(lock)));
}

auto local_for_null::make(const basic_lock& lock) -> local_of<null_tag> {
	return cast_out(js::transfer_in<v8::Local<iv8::Null>>(nullptr, unwrap_lock_witness(lock)));
}

// boolean
auto local_for_boolean::make(const basic_lock& lock, bool value) -> local_of<boolean_tag> {
	return cast_out(js::transfer_in<v8::Local<v8::Boolean>>(value, unwrap_lock_witness(lock)));
}

value_for_boolean::operator bool() const {
	return js::transfer_out<bool>(cast_in(local_of{*this}), unwrap_lock_witness(lock()));
}

// number
auto local_for_number::inspect() const -> value_typeof {
	auto subject = cast_in(*this);
	if (subject->IsInt32()) {
		return value_typeof::number_i32;
	} else {
		return value_typeof::number;
	}
}

auto local_for_number::make(const basic_lock& lock, double value) -> local_of<number_tag_of<double>> {
	return local_of<number_tag_of<double>>::from(cast_out(js::transfer_in<v8::Local<v8::Number>>(value, unwrap_lock_witness(lock))));
}

auto local_for_number::make(const basic_lock& lock, std::int32_t value) -> local_of<number_tag_of<std::int32_t>> {
	return cast_out(js::transfer_in<v8::Local<v8::Int32>>(value, unwrap_lock_witness(lock)));
}

value_for_number::operator double() const {
	return js::transfer_out<double>(cast_in(local_of{*this}), unwrap_lock_witness(lock()));
}

value_for_number::operator std::int32_t() const {
	return js::transfer_out<std::int32_t>(cast_in(local_of{*this}), unwrap_lock_witness(lock()));
}

// name & string
auto local_for_name::inspect() const -> value_typeof {
	auto subject = cast_in(*this);
	if (subject->IsString()) {
		return local_of<string_tag>::from(*this).inspect();
	} else {
		return value_typeof::symbol;
	}
}

auto local_for_string::is_latin1() const -> bool {
	return cast_in(*this)->IsOneByte();
}

auto local_for_string::inspect() const -> value_typeof {
	auto subject = cast_in(*this);
	if (subject->IsOneByte()) {
		return value_typeof::string_latin1;
	} else {
		return value_typeof::string;
	}
}

auto local_for_string::make(const basic_lock& lock, std::u16string_view value) -> local_of<string_tag_of<char16_t>> {
	return cast_out(js::transfer_in<v8::Local<iv8::StringTwoByte>>(value, unwrap_lock_witness(lock)));
}

auto local_for_string::make(const basic_lock& lock, std::string_view value) -> local_of<string_tag_of<char>> {
	return cast_out(js::transfer_in<v8::Local<iv8::StringOneByte>>(value, unwrap_lock_witness(lock)));
}

value_for_string::operator std::string() const {
	return js::transfer_out<std::string>(cast_in(local_of{*this}), unwrap_lock_witness(lock()));
}

value_for_string::operator std::u16string() const {
	return js::transfer_out<std::u16string>(cast_in(local_of{*this}), unwrap_lock_witness(lock()));
}

// bigint
auto local_for_bigint::inspect() const -> value_typeof {
	auto subject = cast_in(*this);
	// NOLINTNEXTLINE(cppcoreguidelines-init-variables)
	bool lossless;
	subject->Int64Value(&lossless);
	if (lossless) {
		return value_typeof::bigint_i64;
	} else {
		return value_typeof::bigint;
	}
}

auto local_for_bigint::make(const runtime_lock& lock, js::bigint value) -> local_of<bigint_tag_of<js::bigint>> {
	return local_of<bigint_tag_of<js::bigint>>::from(cast_out(js::transfer_in<v8::Local<v8::BigInt>>(std::move(value), unwrap_lock_witness(lock))));
}

auto local_for_bigint::make(const basic_lock& lock, std::int64_t value) -> local_of<bigint_tag_of<std::int64_t>> {
	return cast_out(js::transfer_in<v8::Local<iv8::BigInt64>>(value, unwrap_lock_witness(lock)));
}

value_for_bigint::operator js::bigint() const {
	return js::transfer_out<js::bigint>(cast_in(local_of{*this}), unwrap_lock_witness(lock()));
}

value_for_bigint::operator std::int64_t() const {
	return js::transfer_out<std::int64_t>(cast_in(local_of{*this}), unwrap_lock_witness(lock()));
}

} // namespace isolated_vm
