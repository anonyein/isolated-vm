module;
#include "auto_js/export_tag.h"
export module isolated_vm:value.record;
import :value.vector;

namespace isolated_vm {
using namespace js;

// local_of<record_tag>
class EXPORT local_for_record : public local_next<record_tag> {
	public:
		using local_next<record_tag>::local_next;
		auto set(const runtime_lock& lock, local_of<> key, local_of<> value) const -> void;
		static auto make(const runtime_lock& lock) -> local_of<record_tag>;
};

// value_of<record_tag>
class EXPORT value_for_record : public value_next<record_tag> {
	private:
		using internal_keys_type = value_of<vector_tag>;

	public:
		using value_next<record_tag>::value_next;
		using key_type = local_of<primitive_tag>;
		using mapped_type = local_of<>;
		using value_type = std::pair<key_type, mapped_type>;

	private:
		class EXPORT iterator_transform {
			public:
				explicit iterator_transform(const value_for_record& subject);
				auto operator()(local_of<> key) const -> value_type;

			private:
				const value_for_record* subject_;
		};

	public:
		using range_type = std::ranges::transform_view<std::views::all_t<const internal_keys_type&>, iterator_transform>;
		using iterator = std::ranges::iterator_t<range_type>;

		[[nodiscard]] auto get(local_of<name_tag> key) const -> local_of<>;
		[[nodiscard]] auto get(local_of<number_tag> key) const -> local_of<>;
		[[nodiscard]] auto get(local_of<primitive_tag> key) const -> local_of<>;
		[[nodiscard]] auto has(local_of<name_tag> key) const -> bool;
		[[nodiscard]] auto has(local_of<number_tag> key) const -> bool;
		[[nodiscard]] auto has(local_of<primitive_tag> key) const -> bool;
		[[nodiscard]] auto into_range() const -> range_type;
		[[nodiscard]] auto size() const -> std::size_t;

	private:
		[[nodiscard]] auto keys() const -> const internal_keys_type&;

		mutable internal_keys_type keys_;
};

} // namespace isolated_vm
