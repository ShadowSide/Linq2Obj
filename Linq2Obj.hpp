#pragma once
#include <memory>
#include <exception>
#include <type_traits>
#include <utility>
#include <functional>

namespace l2o
{
	namespace internal_
	{
		/*template<class T>
		typename std::add_lvalue_reference_t<std::add_const_t<std::remove_reference<T>>> decllval() noexcept;*/
		template<class T>
		using to_const_ref_t = std::add_lvalue_reference_t<std::add_const_t<std::remove_reference<T>>>;
	}

	class enumerable_exception : public std::exception
	{
		virtual char const* what() const
		{
			return "enumerable exception";
		}
	};

	class empty_enumerable_exception : public enumerable_exception
	{
		virtual char const* what() const
		{
			return "Empty enumerable";
		}
	};

	template<class T>
	class enumerator
	{
		T& get()
		{

		}

		const T& get() const
		{

		}

		bool isCanGetNext()
		{

		}
	};

	template<class T>
	struct i_enumerable_internal : public std::enable_shared_from_this<i_enumerable_internal<T>>
	{
	public:
		virtual enumerator<T> get_enumerator() const = 0;
		virtual ~i_enumerable_internal() {};
	};

	template<class T>
	class enumerable_internal_base : public i_enumerable_internal<T>
	{
		public:
	};

	template<class T, class ForwardCollectionPointer>
	class enumerable_forward_collection: public enumerable_internal_base<T>
	{
	private:
		ForwardCollectionPointer _source;

		/*ctor s*/
		
	public:
		enumerator<T> get_enumerator() const
		{

		}
	};

	template<class T>
	class enumerable
	{
	private:
		std::shared_ptr<i_enumerable_internal<T>> _impl;

	public:
		using value_type = T;
		
		enumerable()
		{
			_impl = nullptr;
		}

		explicit enumerable(const std::shared_ptr<i_enumerable_internal<T>>& internalEnumerable)
		{
			_impl = internalEnumerable;
		}

		explicit enumerable(std::shared_ptr<i_enumerable_internal<T>>&& internalEnumerable)
		{
			_impl = std::move(internalEnumerable);
		}

		template<class U>
		explicit enumerable(const std::shared_ptr<i_enumerable_internal<U>>& internalEnumerable)
		{
			_impl = internalEnumerable;
		}

		template<class U>
		explicit enumerable(std::shared_ptr<i_enumerable_internal<U>>&& internalEnumerable)
		{
			_impl = std::move(internalEnumerable);
		}

		enumerable(const enumerable& Enumerable)
		{
			_impl = Enumerable._impl;
		}

		enumerable(enumerable&& Enumerable)
		{
			_impl = std::move(Enumerable._impl);
		}

		template<class U, class SFinae = std::enable_if_t<std::is_convertible_v<U, T>>>
		enumerable(const enumerable<U>& Enumerable)
		{
			_impl = Enumerable._impl;
		}

		template<class U, class SFinae = std::enable_if_t<std::is_convertible_v<U, T>>>
		enumerable(enumerable<U>&& Enumerable)
		{
			_impl = std::move(Enumerable._impl);
		}

		enumerable& operator=(const enumerable& Enumerable)
		{
			if (&Enumerable == this)
				return;
			_impl = Enumerable._impl;
			return *this;
		}

		enumerable& operator=(enumerable&& Enumerable)
		{
			if (&Enumerable == this)
				return;
			_impl = std::move(Enumerable._impl);
			return *this;
		}

		template<class U, class SFinae = std::enable_if_t<std::is_convertible_v<U, T>>>
		enumerable& operator=(const enumerable<U>& Enumerable)
		{
			if (&Enumerable == this)
				return;
			_impl = Enumerable._impl;
			return *this;
		}

		template<class U, class SFinae = std::enable_if_t<std::is_convertible_v<U, T>>>
		enumerable& operator=(enumerable<U>&& Enumerable)
		{
			if (&Enumerable == this)
				return;
			_impl = std::move(Enumerable._impl);
			return *this;
		}

		template<class ForwardCollectionPointer>
		explicit enumerable(ForwardCollectionPointer forwardRangeByPointer)
		{
			using ResultItem = T;
			_impl = std::make_shared<enumerable_forward_collection<ResultItem, ForwardCollectionPointer>>(forwardRangeByPointer);
		}

		enumerator<T> get_dangling_enumerator() const
		{
			if (_impl == nullptr)
				throw empty_enumerable_exception();
			return _impl->get_enumerator();
		}

		/*============================================================*/

		template<class Functor>
		auto select(Functor&& selector) -> enumerable<std::/*invoke_result_t*/result_of_t<Functor/*,*/ (l2o::internal_::to_const_ref_t<T>)>>
		{
			using ResultItem = std::/*invoke_result_t*/result_of_t<Functor/*,*/(l2o::internal_::to_const_ref_t<T>)>;
			return enumerable<ResultItem>(std::make_shared<enumerable_select<ResultItem>>(_impl, std::forward<Functor>(selector)));
		}

		template<class Predicate>
		enumerable where(Predicate&& condition)
		{
			using ResultItem = T;
			return enumerable<ResultItem>(std::make_shared<enumerable_where<ResultItem>>(_impl, std::forward<Predicate>(condition)));
		}

		/*============================================================*/		
	};

	template<class T>
	auto from(const enumerable<T>& enumerableCollection) -> enumerable<T>
	{
		using Item = T;
		return enumerable<Item>(enumerableCollection);
	}

	template<class T>
	auto from(enumerable<T>& enumerableCollection) -> enumerable<T>
	{
		using ResultItem = T;
		return enumerable<ResultItem>(enumerableCollection);
	}

	template<class T>
	auto from(enumerable<T>&& enumerableCollection) -> enumerable<T>
	{
		using ResultItem = T;
		return enumerable<ResultItem>(std::move(enumerableCollection));
	}

	template<class ForwardCollection>
	auto from(const std::shared_ptr<ForwardCollection>& rangeBySmartPointer) -> enumerable<std::decay_t<decltype(*std::declval<std::decay_t<ForwardCollection>>().cbegin())>>
	{
		using ResultItem = std::decay_t<decltype(*std::declval<ForwardCollection>.cbegin())>;
		return enumerable<ResultItem>(const_cast<std::shared_ptr<ForwardCollection>&>(rangeBySmartPointer));
	}

	template<class ForwardCollection>
	auto from(std::shared_ptr<ForwardCollection>& rangeBySmartPointer) -> enumerable<std::decay_t<decltype(*std::declval<std::decay_t<ForwardCollection>>().cbegin())>>
	{
		using ResultItem = std::decay_t<decltype(*std::declval<ForwardCollection>.cbegin())>;
		return enumerable<ResultItem>(rangeBySmartPointer);
	}

	template<class ForwardCollection>
	auto from(std::shared_ptr<ForwardCollection>&& rangeBySmartPointer) -> enumerable<std::decay_t<decltype(*std::declval<std::decay_t<ForwardCollection>>().cbegin())>>
	{
		using ResultItem = std::decay_t<decltype(*std::declval<ForwardCollection>.cbegin())>;
		return enumerable<ResultItem>(std::move(rangeBySmartPointer));
	}

	template<class ForwardCollection>
	auto from(std::unique_ptr<ForwardCollection>&& rangeBySmartPointer) -> enumerable<std::decay_t<decltype(*std::declval<std::decay_t<ForwardCollection>>().cbegin())>>
	{
		using ResultItem = std::decay_t<decltype(*std::declval<ForwardCollection>.cbegin())>;
		return enumerable<ResultItem>(std::move(rangeBySmartPointer));
	}

	template<class ForwardCollection>
	auto from(const std::reference_wrapper<ForwardCollection>& rangeByDanglingReference) -> enumerable<std::decay_t<decltype(*std::declval<std::decay_t<ForwardCollection>>().cbegin())>>
	{
		using ResultItem = std::decay_t<decltype(*std::declval<ForwardCollection>.cbegin())>;
		return enumerable<ResultItem>(const_cast<ResultItem*>(&(rangeByDanglingReference.get())));
	}

	template<class ForwardCollection>
	auto from(const ForwardCollection& rangeByDanglingReference) -> enumerable<std::decay_t<decltype(*std::declval<std::decay_t<ForwardCollection>>().cbegin())>>
	{
		using ResultItem = std::decay_t<decltype(*std::declval<ForwardCollection>.cbegin())>;
		return enumerable<ResultItem>(const_cast<ResultItem*>(&(rangeByDanglingReference)));
	}
}