#include "stdafx.h"
#include "..\L2O\l2o.h"
#include <list>
#include <vector>
#include <algorithm>
#include <memory>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace L2OTest
{		
	TEST_CLASS(L2O_SimpleTests)
	{
	public:
		
		TEST_METHOD(TestOfTest)
		{
			std::list a{ { 1,2 } };
			std::vector b{{ 1,2 }};
			Assert::IsTrue(std::equal(a.cbegin(),a.cend(), b.cbegin(), b.cend()));
			Assert::AreEqual(a.size(), b.size());
		}

		TEST_METHOD(ToFromEmptyList)
		{
			std::list<int> a{ };
			auto b = l2o::make_reference_enumerable(a).to_list();
			Assert::AreEqual(a.size(), b.size());
			Assert::IsTrue(std::equal(a.cbegin(), a.cend(), b.cbegin(), b.cend()));
		}

		TEST_METHOD(ToFromList)
		{
			std::list a{ { 1,2 } };
			auto b = l2o::make_reference_enumerable(a).to_list();
			Assert::AreEqual(a.size(), b.size());
			Assert::IsTrue(std::equal(a.cbegin(), a.cend(), b.cbegin(), b.cend()));
		}

		TEST_METHOD(ToFromVector)
		{
			std::vector a{ {1,2} };
			auto b = l2o::make_reference_enumerable(a).to_vector();
			Assert::AreEqual(a.size(), b.size());
			Assert::IsTrue(std::equal(a.cbegin(), a.cend(), b.cbegin(), b.cend()));
		}

		TEST_METHOD(ToRange)
		{
			std::vector a{ {1,2} };
			auto b = l2o::make_reference_enumerable(a).to_range();
			Assert::IsTrue(std::equal(a.cbegin(), a.cend(), b.cbegin(), b.cend()));
		}

		/*TEST_METHOD(ToUnorderedSet)
		{
			std::vector a{ {1, 2, 2} };
			std::vector r{ {1, 2} };
			std::vector r2{ {2, 3} };
			auto b = l2o::make_reference_enumerable(a).to_unordered_set();
			Assert::AreEqual(b.size(), r.size());
			Assert::IsTrue(std::equal(b.cbegin(), b.cend(), r.cbegin(), r.cend()));
			auto b2 = l2o::make_reference_enumerable(a).to_unordered_set([](auto item) {return item + 1; });
			Assert::AreEqual(b2.size(), r2.size());
			Assert::IsTrue(std::equal(b2.cbegin(), b2.cend(), r2.cbegin(), r2.cend()));
		}*/

		/*TEST_METHOD(ToUnorderedMultiSet)
		{
			std::vector a{ {1, 2, 2} };
			std::vector r2{ {2, 3, 3} };
			auto b = l2o::make_reference_enumerable(a).to_unordered_set();
			Assert::AreEqual(b.size(), a.size());
			Assert::IsTrue(std::equal(b.cbegin(), b.cend(), a.cbegin(), a.cend()));
			auto b2 = l2o::make_reference_enumerable(a).to_unordered_set([](auto item) {return item + 1; });
			Assert::AreEqual(b2.size(), r2.size());
			Assert::IsTrue(std::equal(b2.cbegin(), b2.cend(), r2.cbegin(), r2.cend()));
		}*/

		TEST_METHOD(WrongEqualityComparable)
		{
			struct A {};
			std::vector a1{ { A{}, A{} }};
			auto b1 = l2o::make_reference_enumerable(a1).select([](auto item) {return item; });
		}

		TEST_METHOD(Where)
		{
			std::vector source{ {1, 2, 3, 4 } };
			std::vector destination{ {2, 4} };
			auto intermediate = 
				l2o::make_reference_enumerable(source)
				.where([](auto item) {return item % 2 == 0; })
				.to_vector();
			Assert::AreEqual(intermediate.size(), destination.size());
			Assert::IsTrue(std::equal(intermediate.cbegin(), intermediate.cend(), destination.cbegin(), destination.cend()));
		}

		/*TEST_METHOD(WhereWithIndex)
		{
			std::vector source{ {1, 2, 3, 4} };
			std::vector destination{ {2, 4} };
			auto intermediate =
				l2o::make_reference_enumerable(source)
				.where_with_index([](auto item, auto index) {return index % 2 != 0; })
				.to_vector();
			Assert::AreEqual(intermediate.size(), destination.size());
			Assert::IsTrue(std::equal(intermediate.cbegin(), intermediate.cend(), destination.cbegin(), destination.cend()));
		}*/

		TEST_METHOD(Select)
		{
			std::vector source{ {1, 2, 3, 4} };
			std::vector destination{ {2, 3, 4, 5} };
			auto intermediate =
				l2o::make_reference_enumerable(source)
				.select([](auto item) {return item + 1; })
				.to_vector();
			Assert::AreEqual(intermediate.size(), destination.size());
			Assert::IsTrue(std::equal(intermediate.cbegin(), intermediate.cend(), destination.cbegin(), destination.cend()));
		}

		/*TEST_METHOD(SelectWithIndex)
		{
			std::vector source{ {1, 2, 3, 4} };
			std::vector destination{ {1, 3, 5, 7} };
			auto intermediate =
				l2o::make_reference_enumerable(source)
				.select_with_index([](auto item, auto index) {return item + index })
				.to_vector();
			Assert::AreEqual(intermediate.size(), destination.size());
			Assert::IsTrue(std::equal(intermediate.cbegin(), intermediate.cend(), destination.cbegin(), destination.cend()));
		}*/

		/*TEST_METHOD(Equality)
		{
			std::vector a1{ {1, 2, 3, 4} };
			std::vector a2{ {1, 2, 3, 5} };
			std::vector a3{ {1, 2, 3, 4, 5} };
			std::vector a4{ {1, 2, 3, 4} };
			auto b1 = l2o::make_reference_enumerable(a1);
			auto b2 = l2o::make_reference_enumerable(a2);
			auto b3 = l2o::make_reference_enumerable(a3);
			auto b4 = l2o::make_reference_enumerable(a4);
			Assert::AreEqual(b1, b1);
			Assert::AreEqual(b1, b4);
			Assert::AreNotEqual(b1, b2);
			Assert::AreNotEqual(b1, b3);
		}*/

		/*TEST_METHOD(Comparable)
		{
			std::vector a1{ {1, 2, 3, 4} };
			std::vector a2{ {1, 2, 3, 5} };
			std::vector a3{ {1, 2, 3, 4, 5} };
			std::vector a4{ {1, 2, 3, 4} };
			auto b1 = l2o::make_reference_enumerable(a1);
			auto b2 = l2o::make_reference_enumerable(a2);
			auto b3 = l2o::make_reference_enumerable(a3);
			auto b4 = l2o::make_reference_enumerable(a4);
			Assert::IsTrue(b1 < b2);
			Assert::IsTrue(b1 <= b2);
			Assert::IsTrue(b2 > b1);
			Assert::IsTrue(b2 >= b1);
			Assert::IsTrue(b1 >= b4);
			Assert::IsTrue(b1 <= b4);
			Assert::IsTrue(b1 < b3);
			Assert::IsTrue(b3 > b1);
			Assert::IsTrue(b1 <= b3);
			Assert::IsTrue(b3 >= b1);
		}*/

		/*TEST_METHOD(Zip)
		{
			std::vector a1{ {1, 2, 3, 4} };
			std::vector a2{ {1, 2, 3, 4, 5} };
			std::vector a3{ {2, 4, 6, 8} };
			auto b1 = l2o::make_reference_enumerable(a1);
			auto b2 = l2o::make_reference_enumerable(a2);
			auto b3 = b1.zip(b2).to_range();
			Assert::IsTrue(std::equal(b3.cbegin(), b3.cend(), a3.cbegin(), a3.cend()));
		}*/

		/*TEST_METHOD(Concat)
		{
			std::vector a1{ {1, 2, 3, 4} };
			std::vector a2{ {5, 6, 7, 8} };
			std::vector a3{ {1, 2, 3, 4, 5, 6, 7, 8} };
			auto b1 = l2o::make_reference_enumerable(a1);
			auto b2 = l2o::make_reference_enumerable(a2);
			auto b3 = b1.concat(b2).to_range();
			Assert::IsTrue(std::equal(b3.cbegin(), b3.cend(), a3.cbegin(), a3.cend()));
		}*/

		TEST_METHOD(Take)
		{
			std::vector a1{ {1, 2, 3, 4} };
			std::vector<int> a2{ };
			std::vector a3{ {1, 2, 3 } };
			auto b1 = 
				l2o::make_reference_enumerable(a1)
				.take(10)
				.to_range();
			Assert::IsTrue(std::equal(b1.cbegin(), b1.cend(), a1.cbegin(), a1.cend()));
			auto b2 =
				l2o::make_reference_enumerable(a1)
				.take(0)
				.to_range();
			Assert::IsTrue(std::equal(b2.cbegin(), b2.cend(), a2.cbegin(), a2.cend()));
			auto b3 =
				l2o::make_reference_enumerable(a1)
				.take(3)
				.to_range();
			Assert::IsTrue(std::equal(b3.cbegin(), b3.cend(), a3.cbegin(), a3.cend()));
		}

		/*TEST_METHOD(Skip)
		{
			std::vector a1{ {1, 2, 3, 4} };
			std::vector<int> a2{ };
			std::vector a3{ {2, 3, 4 } };
			auto b1 =
				l2o::make_reference_enumerable(a1)
				.skip(0)
				.to_range();
			Assert::IsTrue(std::equal(b1.cbegin(), b1.cend(), a1.cbegin(), a1.cend()));
			auto b2 =
				l2o::make_reference_enumerable(a1)
				.skip(10)
				.to_range();
			Assert::IsTrue(std::equal(b2.cbegin(), b2.cend(), a2.cbegin(), a2.cend()));
			auto b3 =
				l2o::make_reference_enumerable(a1)
				.skip(1)
				.to_range();
			Assert::IsTrue(std::equal(b3.cbegin(), b3.cend(), a3.cbegin(), a3.cend()));
		}*/

		/*TEST_METHOD(Aggregate)
		{
			std::vector a1{ {1, 2, 3, 4} };
			auto b1 =
				l2o::make_reference_enumerable(a1)
				.aggregate(0, [](auto accumulator, auto item) {return accumulator + item; }, [](auto accumulator) {return accumulator + 1; });
			Assert::AreEqual(b1 == 11);
			auto b2 =
				l2o::make_reference_enumerable(a1)
				.aggregate(0, [](auto accumulator, auto item) {return accumulator + item; });
			Assert::AreEqual(b1 == 10);
		}*/

		TEST_METHOD(CopyEnumerable)
		{
			std::vector a1{ {1, 2, 3, 4} };
			auto b1 = l2o::make_reference_enumerable(a1);
			auto b2 = b1;
			auto c1 = b1.to_range();
			auto c2 = b2.to_range();
			Assert::IsTrue(std::equal(c1.cbegin(), c1.cend(), c2.cbegin(), c2.cend()));
		}

		/*TEST_METHOD(CopyEnumerable2)
		{
			std::vector a1{ {1, 2, 3, 4} };
			auto b1 = l2o::make_reference_enumerable(a1);
			auto b2 = b1.skip(1);
			auto b3 = b1.skip(1);
			auto c2 = b2.to_range();
			auto c3 = b3.to_range();
			Assert::IsTrue(std::equal(b1.cbegin(), b1.cend(), c2.cbegin(), c2.cend()));
			Assert::IsTrue(std::equal(c2.cbegin(), c2.cend(), c3.cbegin(), c3.cend()));
		}*/

		TEST_METHOD(CopyRange)
		{
			std::vector a1{ {1, 2, 3, 4 } };
			auto b1 = l2o::make_reference_enumerable(a1);
			auto c1 = b1.to_range();
			auto c2 = c1;
			Assert::IsTrue(std::equal(c1.cbegin(), c1.cend(), c2.cbegin(), c2.cend()));
		}

		TEST_METHOD(FromShared)
		{
			l2o::enumerable<int> b0;
			{
				auto a1 = std::make_shared<std::vector<int>>(1, 2, 3, 4);
				auto b1 = l2o::make_enumerable(a1);
				b0 = b1;
			}
			std::vector c1{ {1, 2, 3, 4} };
			auto d1 = b0.to_range();
			Assert::IsTrue(std::equal(d1.cbegin(), d1.cend(), c1.cbegin(), c1.cend()));
		}

		TEST_METHOD(RangeFromShared)
		{
			l2o::range<int> b0;
			{
				auto a1 = std::make_shared<std::vector<int>>(1, 2, 3, 4);
				auto b1 = l2o::make_enumerable(a1).to_range();
				b0 = b1;
			}
			std::vector c1{ {1, 2, 3, 4} };
			Assert::IsTrue(std::equal(b0.cbegin(), b0.cend(), c1.cbegin(), c1.cend()));
		}

		TEST_METHOD(FromCopy)
		{
			l2o::enumerable<int> b0;
			{
				std::vector a1{ {1, 2, 3, 4} };
				auto b1 = l2o::make_copy_enumerable(a1);
				b0 = b1;
			}
			std::vector c1{ {1, 2, 3, 4} };
			auto d1 = b0.to_range();
			Assert::IsTrue(std::equal(d1.cbegin(), d1.cend(), c1.cbegin(), c1.cend()));
		}

		TEST_METHOD(RangeFromCopy)
		{
			l2o::range<int> b0;
			{
				std::vector a1{ {1, 2, 3, 4} };
				auto b1 = l2o::make_copy_enumerable(a1).to_range();
				b0 = b1;
			}
			std::vector c1{ {1, 2, 3, 4} };
			Assert::IsTrue(std::equal(b0.cbegin(), b0.cend(), c1.cbegin(), c1.cend()));
		}

		/*TEST_METHOD(ToUnorderedMap)
		{
			std::vector a{ {1, 2, 2} };
			std::vector r2{ {2, 3} };
			std::vector r3{ {3, 4} };
			auto b2 = l2o::make_reference_enumerable(a).to_unordered_map([](auto item) {return item + 1; }, [](auto item) {return item + 2; });
			auto b2keys = l2o::make_reference_enumerable(b2).select([](auto item) {return item.first }).to_range();
			auto b2values = l2o::make_reference_enumerable(b2).select([](auto item) {return item.second }).to_range();
			Assert::AreEqual(b2keys.size(), r2.size());
			Assert::IsTrue(std::equal(b2keys.cbegin(), b2keys.cend(), r2.cbegin(), r2.cend()));
			Assert::AreEqual(b2values.size(), r3.size());
			Assert::IsTrue(std::equal(b2values.cbegin(), b2values.cend(), r3.cbegin(), r3.cend()));
		} */

		/*TEST_METHOD(ToUnorderedMultiMap)
		{
			std::vector a{ {1, 2, 2} };
			std::vector r2{ {2, 3, 3} };
			std::vector r3{ {3, 4, 4} };
			auto b2 = l2o::make_reference_enumerable(a).to_unordered_map([](auto item) {return item + 1; }, [](auto item) {return item + 2; });
			auto b2keys = l2o::make_reference_enumerable(b2).select([](auto item) {return item.first }).to_range();
			auto b2values = l2o::make_reference_enumerable(b2).select([](auto item) {return item.second }).to_range();
			Assert::AreEqual(b2keys.size(), r2.size());
			Assert::IsTrue(std::equal(b2keys.cbegin(), b2keys.cend(), r2.cbegin(), r2.cend()));
			Assert::AreEqual(b2values.size(), r3.size());
			Assert::IsTrue(std::equal(b2values.cbegin(), b2values.cend(), r3.cbegin(), r3.cend()));
		}*/

		/*TEST_METHOD(Distinct)
		{
			std::vector a{ {1, 2, 2} };
			std::vector r{ {2, 3} };
			auto b = l2o::make_reference_enumerable(a).distinct().to_range();
			Assert::AreEqual(b.size(), r.size());
			Assert::IsTrue(std::equal(b.cbegin(), b.cend(), r.cbegin(), r.cend()));
		}*/

		TEST_METHOD(DefaultConstruct)
		{
			l2o::enumerable<int> a;
			l2o::range<int> b;
		}

		/*TEST_METHOD(ToShared)
		{
			l2o::enumerable<std::shared_ptr<int>> b0;
			{
				auto a1 = std::make_shared<std::vector<int>>(1, 2, 3);
				auto b1 = l2o::make_enumerable(a1).to_shared_ptrs();
				b0 = b1;
			}
			std::vector c1{ 1, 2, 3 };
			auto d1 = b0.select([](auto item) {return *item; }).to_range();
			Assert::IsTrue(std::equal(d1.cbegin(), d1.cend(), c1.cbegin(), c1.cend()));
		}

		TEST_METHOD(ToSharedCopy)
		{
			l2o::enumerable<std::shared_ptr<int>> b0;
			{
				std::vector a1{ 1, 2, 3 };
				auto b1 = l2o::make_copy_enumerable(a1).to_shared_ptrs();
				b0 = b1;
			}
			std::vector c1{ 1, 2, 3 };
			auto d1 = b0.select([](auto item) {return *item; }).to_range();
			Assert::IsTrue(std::equal(d1.cbegin(), d1.cend(), c1.cbegin(), c1.cend()));
		}*/

		/*TODO: тесты на копирование итераторов*/
	};
}