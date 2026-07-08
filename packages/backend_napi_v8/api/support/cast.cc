module isolated_vm:support.cast;
import :handle.local_of;
import auto_js;
import v8_js;
import std;

namespace isolated_vm {
using namespace js;

template <class Tag>
auto cast_in(local_of<Tag> value) -> v8::Local<iv8::tag_to_v8<Tag>> {
	return std::bit_cast<v8::Local<iv8::tag_to_v8<Tag>>>(value);
}

template <class Tag>
auto cast_in(local_next<Tag> value) -> v8::Local<iv8::tag_to_v8<Tag>> {
	return std::bit_cast<v8::Local<iv8::tag_to_v8<Tag>>>(value);
}

template <class Type>
auto cast_out(v8::Local<Type> value) -> local_of<iv8::v8_to_tag<Type>> {
	return std::bit_cast<local_of<iv8::v8_to_tag<Type>>>(value);
}

} // namespace isolated_vm
