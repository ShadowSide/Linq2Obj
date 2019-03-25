#pragma once
#include "l2o_enumerable_algorithms.h"

namespace l2o
{
	template<class Range>
	auto make_reference_enumerable(const Range& range) -> enumerable<std::decay_t<decltype(*(range.cbegin()))>>
	{
		using T = std::decay_t<decltype(*(range.cbegin()))>;
		return std::make_shared<detail::enumerable_impl_from_range<T, Range* >> (&range);
	}

	template<class Range>
	auto make_enumerable(const std::shared_ptr<Range>& range) -> enumerable<std::decay_t<decltype(*(range->cbegin()))>>
	{
		using T = std::decay_t<decltype(*(range->cbegin()))>;
		return std::make_shared<detail::enumerable_impl_from_range<T, std::shared_ptr<Range> >>(range);
	}

	template<class Range>
	auto make_enumerable(std::shared_ptr<Range>&& range) -> enumerable<std::decay_t<decltype(*(range->cbegin()))>>
	{
		using T = std::decay_t<decltype(*(range->cbegin()))>;
		return std::make_shared<detail::enumerable_impl_from_range<T, std::shared_ptr<Range> >>(std::move(range));
	}

	template<class T>
	auto make_enumerable(const enumerable<T>& enumerable_) -> enumerable<T>
	{
		return enumerable_.get_enumerable();
	}

	template<class T>
	auto make_empty_enumerable() -> enumerable<T>
	{
		using Range = std::vector<T>;
		auto rangePtr = std::make_shared<Range>();
		return std::make_shared<detail::enumerable_impl_from_range<T, std::shared_ptr<Range> >> (std::move(rangePtr));
	}

	template<class Range>
	auto make_copy_enumerable(Range&& range) -> enumerable<std::decay_t<decltype(*(range.cbegin()))>>
	{
		using T = std::decay_t<decltype(*(range.cbegin()))>;
		return std::make_shared<detail::enumerable_impl_from_range<T, std::shared_ptr<Range> >> (std::make_shared(std::forward<Range>(range)));
	}

	/*template<class Range>
	auto make_cached_source_enumerable(const Range& range) -> enumerable<std::decay_t<decltype(*(range.cbegin()))>>
	{
		using T = std::decay_t<decltype(*(range.cbegin()))>;
		using NewRange = std::vector<T>;
		var rangePtr = std::make_shared<NewRange>(range.cbegin(), range.cend());
		return std::make_shared<detail::enumerable_impl_from_range<T, std::shared_ptr<Range> >>(rangePtr);
	}

	template<class Range>
	auto make_cached_source_enumerable(Range&& range) -> enumerable<std::decay_t<decltype(*(range.cbegin()))>>
	{
		using T = std::decay_t<decltype(*(range.cbegin()))>;
		using NewRange = std::vector<T>;
		var rangePtr = std::make_shared<NewRange>(std::make_move_iterator(range.begin()), std::make_move_iterator(range.end()));
		//range.clear();
		return std::make_shared<detail::enumerable_impl_from_range<T, std::shared_ptr<Range> >>(rangePtr);
	}

	template<class T>
	auto make_cached_source_enumerable(std::vector<T>&& range) -> enumerable<std::decay_t<decltype(*(range.cbegin()))>>
	{
		using T = std::decay_t<decltype(*(range.cbegin()))>;
		using NewRange = std::vector<T>;
		var rangePtr = std::make_shared<NewRange>(std::move(range));
		return std::make_shared<detail::enumerable_impl_from_range<T, std::shared_ptr<Range> >>(rangePtr);
	}*/
}