#pragma once

#include <optional>
#include <memory>

namespace l2o
{
	template<class T>
	struct ienumerator
	{
		virtual std::optional<T> get() = 0;
		virtual std::shared_ptr<ienumerator<T>> clone() = 0;
		virtual ~ienumerator() {}
	};

	namespace detail
	{
		template<class T, class MostDerived>
		class enumerator_base : public ienumerator<T>
		{
		public:
			virtual std::shared_ptr<ienumerator<T>> clone()
			{
				return std::make_shared<MostDerived>(*static_cast<const MostDerived*>(this));
			}
		};

		template <class T, class NextFromEnumerator>
		class enumerator_impl : public enumerator_base<T, enumerator_impl<T, NextFromEnumerator>>
		{
		private:
			std::shared_ptr<ienumerator<T>> _source;
			NextFromEnumerator _nextFromEnumerator;

		public:
			enumerator_impl(std::shared_ptr<ienumerator<T>> source, NextFromEnumerator&& nextFromEnumerator)
				: _source(std::move(source))
				, _nextFromEnumerator(std::move(nextFromEnumerator))
			{}

			explicit enumerator_impl(const std::shared_ptr<ienumerator<T>>& cloneable)
				: _source(cloneable._source ? cloneable._source->clone() : nullptr)
				, _nextFromEnumerator(cloneable->nextFromEnumerator)
			{}

			std::optional<T> get()
			{
				if (_source == nullptr)
					return std::logic_error("Not initialized enumerator_impl");
				return _nextFromEnumerator(_source);
			}
		};

		template <class T, class SourcePtr, class SourcePtrIterator>
		class enumerator_impl_from_range : public enumerator_base<T, enumerator_impl_from_range<T, SourcePtr, SourcePtrIterator>>
		{
		private:
			SourcePtr _source;
			SourcePtrIterator _it, _end;

		public:
			enumerator_impl_from_range(const SourcePtr& source, SourcePtrIterator&& begin, SourcePtrIterator&& end)
				: _source(source)
				, _it(std::move(begin))
				, _end(std::move(end))
			{}

			explicit enumerator_impl_from_range(const std::shared_ptr<ienumerator<T>>& cloneable)
				: _source(cloneable->_source)
				, _it(cloneable->_begin)
				, _end(cloneable->_end)
			{}

			std::optional<T> get()
			{
				if (_source == nullptr)
					return std::logic_error("Not initialized enumerator_impl_from_range");
				if (_it != _end)
					return *(_it++);
				return std::nullopt;
			}
		};
	}
}