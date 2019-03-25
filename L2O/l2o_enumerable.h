#pragma once

#include <optional>
#include <memory>
#include "l2o_enumerator.h"

/*TODO: explicit and default constructors*/

namespace l2o
{
	template<class T>
	struct ienumerable
	{
		virtual std::shared_ptr<ienumerator<T>> get_enumerator() = 0;
		virtual ~ienumerable() {}
	};

	namespace detail
	{
		template <class T, class NextFromEnumerator>
		class enumerable_impl : public ienumerable<T>
		{
		private:
			std::shared_ptr<ienumerable<T>> _source;
			NextFromEnumerator _nextFromEnumerator;

			template <class T, class NextFromEnumerator>
			std::shared_ptr<ienumerator<T>> make_enumerator_impl(const std::shared_ptr<ienumerator<T>>& source, const NextFromEnumerator& nextFromEnumerator)
			{
				return std::make_shared<enumerator_impl<std::shared_ptr<ienumerator<T>>, NextFromEnumerator>>(source, nextFromEnumerator)
			}

		public:
			enumerable_impl(std::shared_ptr<ienumerable<T>> source, NextFromEnumerator&& nextFromEnumerator)
				: _source(std::move(source))
				, _nextFromEnumerator(std::move(nextFromEnumerator))
			{}

			std::shared_ptr<ienumerator<T>> get_enumerator()
			{
				if (_source == nullptr)
					return std::logic_error("Not initialized enumerable_impl");
				return make_enumerator_impl(_source->get_enumerator(), _nextFromEnumerator);
			}
		};

		template<class T, class SourcePtr>
		class enumerable_impl_from_range : public ienumerable<T>
		{
		private:
			using SourcePtrIterator = decltype(*(std::declval<SourcePtr>()->cbegin()));
			SourcePtr _source;

			static SourcePtrIterator begin(const SourcePtr& range)
			{
				return range->cbegin();
			}

			static SourcePtrIterator end(const SourcePtr& range)
			{
				return range->cend();
			}

			template <class T>
			std::shared_ptr<ienumerator<T>> make_enumerator_impl(const SourcePtr& source)
			{
				return std::make_shared<enumerator_impl_from_range<T, SourcePtr, SourcePtrIterator>>(source, begin(source), end(source))
			}

		public:
			template<class SourcePtr_, class SFINAE=std::enable_if_t<std::is_convertible_t< SourcePtr_, std::shared_ptr<ienumerable<T>> >> >
			enumerable_impl_from_range(SourcePtr_&& source) 
				: _source(std::forward<SourcePtr_>(source))
			{}

			std::shared_ptr<ienumerator<T>> get_enumerator()
			{
				if (_source == nullptr)
					return std::logic_error("Not initialized enumerable_impl_from_range");
				return make_enumerator_impl<T>(_source);
			}
		};
	}
}