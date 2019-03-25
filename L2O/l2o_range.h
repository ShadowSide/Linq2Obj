#pragma once
#include <optional>
#include <memory>
#include "l2o_enumerator.h"
#include "l2o_enumerable.h"
#include "l2o_range.h"

namespace l2o
{
	namespace detail
	{
		/*TODO: Для итераторов нужна промежуточная абстракция
		
		Тк клонинг данных
		итератор на std::unique_ptr не взлятит*/
		template<class T>
		class iterator_from_enumerator: public std::iterator<std::forward_iterator_tag,
			T, std::ptrdiff_t, const T*, const T&>
		{
		private:
			std::shared_ptr<ienumerator<T>> _source;
			std::optional<T> _valueHolder;

		public:
			using value_type = T;
			using size_type = std::size_t;
			using const_reference = const value_type&;
			using const_pointer = const value_type *;
			using iterator = iterator_from_enumerator<T>;

			iterator_from_enumerator(std::shared_ptr<ienumerator<T>>&& source)
				: _source(std::move(source))
			{
				if(_source)
					_valueHolder = std::move(_source->get());
			}

			iterator_from_enumerator() = default;

			iterator_from_enumerator(const iterator_from_enumerator& source)
				: _source(source._source ? source._source->clone() : nullptr)
				, _valueHolder(source._valueHolder)
			{
				if (_source && !_valueHolder)
					_valueHolder = std::move(_source->get());
			}

			iterator_from_enumerator(iterator_from_enumerator&& source)
				: _source(std::move(source))
				, _valueHolder(std::move(source._valueHolder))
			{
				if (_source && !_valueHolder)
					_valueHolder = std::move(_source->get());
			}

			const iterator_from_enumerator& operator=(const iterator_from_enumerator& source)
			{
				if (&source == this)
					return;
				_source = source._source ? source._source->clone() : nullptr;
				_valueHolder = _source.valueHolder;
				if (_source && !_valueHolder)
					_valueHolder = std::move(_source->get());
				return *this;
			}

			const iterator_from_enumerator& operator=(iterator_from_enumerator&& source)
			{
				if (&source == this)
					return;
				_source = std::move(source._source);
				_valueHolder = std::move(_source.valueHolder);
				if (_source && !_valueHolder)
					_valueHolder = std::move(_source->get());
				return *this;
			}

			const_reference operator*() const
			{
				if (!_valueHolder)
					throw std::logic_error("Iterator not referenced to value");
				return *_valueHolder;
			}

			const iterator_from_enumerator& operator++() const
			{
				if(!_source)
					throw std::logic_error("Not initialized iterator_from_enumerator");
				_valueHolder = std::move(_source->get());
				return *this;
			}

			iterator_from_enumerator operator++(int) const
			{
				if (!_source)
					throw std::logic_error("Not initialized iterator_from_enumerator");
				iterator_from_enumerator result(std::move(*this));
				_source = result._source ? result._source->clone() : nullptr;
				if (_source)
					_valueHolder = std::move(_source->get());
				return std::move(result);
			}
		};
	}

	template<class T>
	class range
	{
	private:
		std::shared_ptr<ienumerable<T>> _source;

	public:
		using value_type = T;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;
		using reference = value_type & ;
		using const_reference = const value_type&;
		using pointer = value_type * ;
		using const_pointer = const value_type * ;
		using const_iterator = detail::iterator_from_enumerator<value_type>;
		using iterator = const_iterator;

		range(std::shared_ptr<ienumerable<T>>&& source)
			: _source(std::move(source))
		{}

		range() = default;

		const_iterator cbegin() const
		{
			return detail::iterator_from_enumerator<value_type>(get_enumerator());
		}

		const_iterator cend() const
		{
			return detail::iterator_from_enumerator<value_type>();
		}

		const_iterator begin() const
		{
			return detail::iterator_from_enumerator<value_type>(get_enumerator());
		}

		const_iterator end() const
		{
			return detail::iterator_from_enumerator<value_type>();
		}

		std::shared_ptr<ienumerator<T>> get_enumerator() const
		{
			if (_source == nullptr)
				throw std::logic_error("Not initialized range");
			return _source->get_enumerator();
		}

		std::shared_ptr<ienumerable<T>> get_enumerable() const
		{
			if (_source == nullptr)
				return std::logic_error("Not initialized range");
			return _source;
		}
	};
}