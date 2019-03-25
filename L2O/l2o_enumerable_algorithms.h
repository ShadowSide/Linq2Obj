#pragma once

#include <optional>
#include <memory>
#include <vector>
#include <list>
#include <set>
#include "l2o_enumerator.h"
#include "l2o_enumerable.h"
#include "l2o_range.h"

namespace l2o
{
	template<class T>
	class enumerable/*: public ienumerable<T>*/
	{
	private:
		std::shared_ptr<ienumerable<T>> _source;

		template <class T, class NextFromEnumerator>
		std::shared_ptr<ienumerable<T>> make_enumerable_impl(const std::shared_ptr<ienumerable<T>>& source, NextFromEnumerator&& nextFromEnumerator)
		{
			return std::make_shared<detail::enumerable_impl<std::shared_ptr<ienumerable<T>>, NextFromEnumerator>>(source, std::forward<NextFromEnumerator>(nextFromEnumerator));
		}

	public:
		enumerable(const std::shared_ptr<ienumerable<T>>& source)
			: _source(source)
		{}

		enumerable() = default;

		std::shared_ptr<ienumerator<T>> get_enumerator() const
		{
			if (_source == nullptr)
				throw std::logic_error("Not initialized enumerable");
			return _source->get_enumerator();
		}

		std::shared_ptr<ienumerable<T>> get_enumerable() const
		{
			if (_source == nullptr)
				throw std::logic_error("Not initialized enumerable");
			return _source;
		}

		range<T> to_range()
		{
			return range(get_enumerable());
		}

		template<class Collection>
		Collection to_collection()
		{
			auto range = to_range();
			return Collection { range.cbegin(), range.cend() };
		}

		std::list<T> to_list()
		{
			return to_collection<std::list<T>>();
		}

		std::vector<T> to_vector()
		{
			return to_collection<std::vector<T>>();
		}

		std::set<T> to_set()
		{
			return to_collection<std::set<T>>();
		}

		template<class Predicate>
		enumerable<T> where(Predicate&& predicate)
		{
			return make_enumerable_impl(get_enumerable(), [predicate = std::forward<Predicate>(predicate)] (const std::shared_ptr<ienumerator<T>>& enumeratorPtr) mutable
			{
				do
				{
					auto value = enumeratorPtr->get();
					if (!value)
						return std::make_optional<T>();
					if (!predicate(*value))
						continue;
					return std::move(value);
				} while (true);
			});
		}

		template<class Functor>
		enumerable<T> select(Functor&& selector)
		{
			return make_enumerable_impl(get_enumerable(), [selector = std::forward<Functor>(selector)] (const std::shared_ptr<ienumerator<T>>& enumeratorPtr) mutable
			{
				auto value = enumeratorPtr->get();
				if (!value)
					return std::make_optional<T>();
				return std::make_optional<T>(selector(*(std::move(value))));
			});
		}

		enumerable<T> take(size_t n)
		{
			return make_enumerable_impl(get_enumerable(), [n] (const std::shared_ptr<ienumerator<T>>& enumeratorPtr) mutable
			{
				while (n > 0)
				{
					--n;
					auto value = enumeratorPtr->get();
					if (!value)
						return std::make_optional<T>();
					return std::move(value);
				}
			});
		}
	};
}
