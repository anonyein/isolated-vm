module isolated_vm;
import :support.cast;
import :support.lock;
import :value;
import auto_js;
import v8_js;
import std;

namespace isolated_vm {
using namespace js;

// value_for_vector
auto value_for_vector::begin() const -> iterator {
	return iterator{*this, 0};
}

auto value_for_vector::end() const -> iterator {
	return iterator{*this, size()};
}

auto value_for_vector::size() const -> std::uint32_t {
	if (size_ == 0) {
		size_ = cast_in(local_of{*this})->Length() + 1;
	}
	return size_ - 1;
}

// value_for_vector::iterator
value_for_vector::iterator::iterator(value_for_vector subject, size_type index) :
		subject_{subject},
		index_{index} {}

auto value_for_vector::iterator::operator*() const -> value_type {
	auto context = unwrap_lock_witness(subject_.lock()).context();
	return cast_out(iv8::unmaybe(cast_in(local_of{subject_})->Get(context, index_)));
}

} // namespace isolated_vm
