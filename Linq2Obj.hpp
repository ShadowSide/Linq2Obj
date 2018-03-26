#pragma once
#include <memory>
#include <exception>
#include <type_traits>
#include <utility>
#include <functional>
#include <vector>

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
	struct i_enumerator
	{
		virtual T& get() = 0;
		virtual const T& get() const = 0;
		virtual bool isCanGetNext() = 0;
		virtual ~i_enumerator(){}
	};

	template<class T>
	struct enumerator_base : i_enumerator<T>
	{
		const T& get() const
		{
			return const_cast<enumerator_base*>(this)->get();
		}
	};

	template<class T>
	class enumerator: public enumerator_base<T>
	{
	private:
		std::shared_ptr<i_enumerator<T>> _impl;

	public:
		enumerator(std::shared_ptr<i_enumerator<T>> impl)
			: _impl(impl)
		{}

		enumerator() = default;
		enumerator(enumerator&&) = default;
		enumerator(const enumerator&) = default;
		enumerator& operator=(const enumerator&) = default;
		enumerator& operator=(enumerator&&) = default;

		T& get()
		{
			return _impl->get();
		}

		bool isCanGetNext()
		{
			return _impl->isCanGetNext();
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

		enumerable_forward_collection() = default;
		enumerable_forward_collection(enumerable_forward_collection&&) = default;
		enumerable_forward_collection(const enumerable_forward_collection&) = default;
		enumerable_forward_collection& operator=(const enumerable_forward_collection&) = default;
		enumerable_forward_collection& operator=(enumerable_forward_collection&&) = default;
		
	public:
		enumerator<T> get_enumerator() const
		{
			if (_impl == nullptr)
				throw empty_enumerable_exception();
			using SourceType = value_type;
			using ResultItem = SourceType;
			struct emumerator_forward_collection : enumerator_base<ResultItem>
			{
				emumerator_forward_collection(ForwardCollectionPointer source)
					: _source(source)
					, _sourceCurrent(std::cbegin(_source))
				{}
				emumerator_forward_collection() = default;
				emumerator_forward_collection(emumerator_forward_collection&&) = default;
				emumerator_forward_collection(const emumerator_forward_collection&) = default;
				emumerator_forward_collection& operator=(const emumerator_forward_collection&) = default;
				emumerator_forward_collection& operator=(emumerator_forward_collection&&) = default;

				ForwardCollectionPointer _source;
				ForwardCollectionPointer _sourceCurrent;

				ResultItem& get()
				{
					return *_sourceCurrent;
				}

				bool isCanGetNext()
				{
					auto isCanGetNext = _sourceCurrent != std::cend(_source);
					if (isCanGetNext)
						++_sourceCurrent;
					return _sourceCurrent;
				}
			};
			return enumerator<ResultItem>(std::make_shared<emumerator_forward_collection>(_impl));
		}
	};

	template<class ResultItem, class SourceItem>
	class enumerable_emumerator_converter: public enumerable_internal_base<ResultItem>
	{
	private:
		//using ResultItem = T;
		using InternalSource = std::shared_ptr<i_enumerable_internal<SourceItem>>;
		InternalSource _source;
		using InternalDestination = std::shared_ptr<i_enumerator<ResultItem>>;
		typedef InternalDestination(InternalSource) EnumeratorConvertorSignature;
		using EnumeratorConvertor = std::function<EnumeratorConvertorSignature>;
		EnumeratorConvertor _converter;

	public:
		template<class InternalSource, class EnumeratorConvertor>
		enumerable_emumerator_converter(InternalSource&& source; EnumeratorConvertor&& converter)
			: _source(std::forward<InternalSource>(source))
			, _converter(std::forward<Converter>(converter))
		{}
		enumerable_emumerator_converter() = delete;
		enumerable_emumerator_converter(enumerable_emumerator_converter&&) = default;
		enumerable_emumerator_converter(const enumerable_emumerator_converter&) = default;
		enumerable_emumerator_converter& operator=(const enumerable_emumerator_converter&) = default;
		enumerable_emumerator_converter& operator=(enumerable_emumerator_converter&&) = default;

		enumerator<T> get_enumerator() const
		{
			return _converter(_source->get_enumerator());
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

		enumerator<T> get_enumerator() const
		{
			if (_impl == nullptr)
				throw empty_enumerable_exception();
			return _impl->get_enumerator();
		}

		/*============================================================*/

		/*template<class Selector>
		enumerable order_by(Selector&& selector)
		{
			return order_by_comparator(ascending_comparator(std::forward<Selector>(selector)));
		}

		template<class Selector>
		enumerable order_by_descending(Selector&& selector)
		{
			return order_by_comparator(descending_comparator(std::forward<Selector>(selector)));
		}

		template<class Selector, class Comparator>
		enumerable order_by_comparator(Comparator&& comparator)
		{
			using ResultItem = T;
			return enumerable<ResultItem>(std::make_shared<enumerable_order_by_comparator<ResultItem>>(_impl, std::forward<Comparator>(comparator)));
		}*/

		//then_by

		template<class Action>
		void foreach(Action&& action)
		{
			auto&& enumerator = _impl->get_enumerator();
			while(enumerator.isCanGetNext())
				action(enumerator->get());
		}

		template<class ResultItem, class EnumeratorConvertor>
		enumerable<ResultItem> emumerator_converter(EnumeratorConvertor&& enumeratorConvertor)
		{
			using SourceType = value_type;
			return enumerable<ResultItem>(std::make_shared<enumerable_emumerator_converter<ResultItem, SourceType>>(_impl, std::forward<EnumeratorConvertor>(enumeratorConvertor)));
		}

		template<class Functor>
		auto select(Functor&& selector) -> enumerable<std::result_of_t<Functor(l2o::internal_::to_const_ref_t<T>)>>
		{
			using SourceItem = T;
			using ResultItem = std::result_of_t<Functor(l2o::internal_::to_const_ref_t<T>)>;
			return emumerator_converter<ResultItem, SourceItem>([selector = std::forward<Functor>(selector)]mutable(enumerator<SourceItem>& source)
			{
				struct enumerator_impl: enumerator_base<ResultItem>
				{
					/*while (source.isCanGetNext())
					{
						auto&& sourceItem = source->get();
						yield(selector(sourceItem));
					}*/
					template<class Selector>
					enumerator_impl(enumerator<SourceItem>& source, Selector&& selector)
						: _source(source)
						, _selector(std::forward<Selector>(selector))
					{}

					enumerator<SourceItem> _source;
					std::decay<std::remove_reference<Selector>>> _selector;

					ResultItem& get()
					{
						return _selector(_impl->get());
					}

					bool isCanGetNext()
					{
						return _source.isCanGetNext();
					}
				};
				return enumerator<ResultItem>(std::make_shared<enumerator_impl>(source, std::forward<Functor>(selector)));
			});
		}

		template<class Predicate>
		enumerable where(Predicate&& condition)
		{
			using SourceItem = T;
			using ResultItem = SourceItem;
			return emumerator_converter<ResultItem, SourceItem>([condition=std::forward<Predicate>(condition)]mutable(enumerator<SourceItem>& source)
			{
				/*while (source.isCanGetNext())
				{
					auto&& sourceItem = source->get();
					if(condition(sourceItem))
						yield(sourceItem);
				}*/
				struct enumerator_impl : enumerator_base<ResultItem>
				{
					template<class Predicate>
					enumerator_impl(enumerator<SourceItem>& source, Predicate&& condition)
						: _source(source)
						, _condition (std::forward<Predicate>(condition))
					{}

					enumerator<SourceItem> _source;
					std::decay<std::remove_reference<Predicate>>> _condition ;

					ResultItem& get()
					{
						return _source->get();
					}

					bool isCanGetNext()
					{
						while (_source.isCanGetNext())
						{
							if (condition(sourceItem))
								return true;
						}
						return false;
					}
				};
				return enumerator<ResultItem>(std::make_shared<enumerator_impl>(source, std::forward<Predicate>(condition)));
			});
		}

		/*============================================================*/		
	};

	template<class T>
	auto from() -> enumerable<T>
	{
		using ResultItem = T;
		return enumerable<ResultItem>(std::make_shared<std::vector>());
	}

	template<class T>
	auto from(const enumerable<T>& enumerableCollection) -> enumerable<T>
	{
		using ResultItem = T;
		return enumerable<ResultItem>(enumerableCollection);
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
