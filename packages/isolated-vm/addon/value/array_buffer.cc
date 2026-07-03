module;
#include "auto_js/export_tag.h"
export module isolated_vm:value.array_buffer;
import :handle.local_of;
import :handle.value_of;
import :value.object;
import auto_js;
import std;

namespace isolated_vm {
using namespace js;

// local_of<data_block_tag>
class EXPORT local_for_data_block : public local_next<data_block_tag> {
	public:
		using local_next<data_block_tag>::local_next;
		[[nodiscard]] auto inspect() const -> value_typeof;
};

// value_of<data_block_tag>
class EXPORT value_for_data_block : public value_next<data_block_tag> {
	public:
		using value_next<data_block_tag>::value_next;
		[[nodiscard]] auto byte_length() const -> std::size_t;
		[[nodiscard]] auto data() const -> std::byte*;
		explicit operator js::array_buffer() const;
		explicit operator std::span<std::byte>() const;
};

// local_of<array_buffer_tag>
class EXPORT local_for_array_buffer : public local_next<array_buffer_tag> {
	public:
		using local_next<array_buffer_tag>::local_next;
		static auto make(const runtime_lock& lock, js::array_buffer buffer) -> local_of<array_buffer_tag>;
};

// local_of<array_buffer_view_tag>
class EXPORT local_for_array_buffer_view : public local_next<array_buffer_view_tag> {
	public:
		using local_next<array_buffer_view_tag>::local_next;
		[[nodiscard]] auto inspect() const -> value_typeof;
};

// value_of<typed_array_tag_of<Type>>
template <class Type>
class EXPORT value_for_typed_array_of : public value_next<typed_array_tag_of<Type>> {
	public:
		using value_next<typed_array_tag_of<Type>>::value_next;
		[[nodiscard]] auto buffer() const -> local_of<data_block_tag>;
		[[nodiscard]] auto byte_offset() const -> std::size_t;
		[[nodiscard]] auto size() const -> std::size_t;
};

} // namespace isolated_vm
