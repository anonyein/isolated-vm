export module isolated_vm:handle.types;
import auto_js;

namespace isolated_vm {
using namespace js;

// `value_handle` is the base class of `local_of<T>`, and `value_of<T>`.
class value_handle {
	protected:
		explicit value_handle(void* value) : value_{value} {}

	public:
		value_handle() = default;

		// Check empty value
		explicit operator bool() const { return value_ != nullptr; }

	private:
		void* value_{};
};

// `local_of` forward declarations
export template <class Tag = value_tag>
class local_of;

template <class Tag>
class local_next;

// `value_of` forward declarations
export template <class Tag>
class value_of;

template <class Tag>
class value_next;

template <class Type>
class value_for_typed_array_of;

// Map of `local_of<Tag>` / `value_of<Tag>` to concrete implementation classes.
template <class Tag>
struct value_defaults {
		using bound_type = value_next<Tag>;
		using value_type = local_next<Tag>;
};

template <class Tag>
struct value_specialization : value_defaults<Tag> {};

// Typed specializations
template <>
struct value_specialization<value_tag> : value_defaults<value_tag> {
		using value_type = class local_for_value;
};

template <>
struct value_specialization<primitive_tag> : value_defaults<primitive_tag> {
		using value_type = class local_for_primitive;
};

template <>
struct value_specialization<undefined_tag> : value_defaults<undefined_tag> {
		using value_type = class local_for_undefined;
};

template <>
struct value_specialization<null_tag> : value_defaults<null_tag> {
		using value_type = class local_for_null;
};

template <>
struct value_specialization<boolean_tag> : value_defaults<boolean_tag> {
		using bound_type = class value_for_boolean;
		using value_type = class local_for_boolean;
};

template <>
struct value_specialization<number_tag> : value_defaults<number_tag> {
		using bound_type = class value_for_number;
		using value_type = class local_for_number;
};

template <>
struct value_specialization<name_tag> : value_defaults<name_tag> {
		using value_type = class local_for_name;
};

template <>
struct value_specialization<string_tag> : value_defaults<string_tag> {
		using bound_type = class value_for_string;
		using value_type = class local_for_string;
};

template <>
struct value_specialization<bigint_tag> : value_defaults<bigint_tag> {
		using bound_type = class value_for_bigint;
		using value_type = class local_for_bigint;
};

template <>
struct value_specialization<function_tag> : value_defaults<function_tag> {
		using value_type = class local_for_function;
};

template <>
struct value_specialization<object_tag> : value_defaults<object_tag> {
		using value_type = class local_for_object;
};

template <>
struct value_specialization<record_tag> : value_defaults<record_tag> {
		using bound_type = class value_for_record;
		using value_type = class local_for_record;
};

template <>
struct value_specialization<list_tag> : value_defaults<list_tag> {
		using value_type = class local_for_array;
};

template <>
struct value_specialization<vector_tag> : value_defaults<vector_tag> {
		using bound_type = class value_for_vector;
};

template <>
struct value_specialization<data_block_tag> : value_defaults<data_block_tag> {
		using bound_type = class value_for_data_block;
		using value_type = class local_for_data_block;
};

template <>
struct value_specialization<array_buffer_tag> : value_defaults<array_buffer_tag> {
		using value_type = class local_for_array_buffer;
};

template <>
struct value_specialization<array_buffer_view_tag> : value_defaults<array_buffer_view_tag> {
		using value_type = class local_for_array_buffer_view;
};

template <>
struct value_specialization<data_view_tag> : value_defaults<data_view_tag> {
		using bound_type = value_for_typed_array_of<void>;
};

template <class Type>
struct value_specialization<typed_array_tag_of<Type>> : value_defaults<typed_array_tag_of<Type>> {
		using bound_type = value_for_typed_array_of<Type>;
};

template <>
struct value_specialization<prototype_tag> : value_defaults<prototype_tag> {
		using value_type = class local_for_prototype;
};

} // namespace isolated_vm
