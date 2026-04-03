#include <gtest/gtest.h>
#include <utility> // Для std::move
#include "LinkedList.hpp"
#include "Exceptions.hpp"
#include "IEnumerator.hpp"

// ============================================================================
// ДЕТЕКТОР УТЕЧЕК ПАМЯТИ (ДЛЯ КАЖДОГО ОТДЕЛЬНОГО УЗЛА СВЯЗНОГО СПИСКА)
// ============================================================================
namespace {

struct InstanceTracker {
    static int activeInstances;
    int value;

    InstanceTracker(int v = 0) : value(v) { activeInstances++; }
    InstanceTracker(const InstanceTracker& other) : value(other.value) { activeInstances++; }
    InstanceTracker(InstanceTracker&& other) noexcept : value(other.value) { 
        other.value = -1;
        activeInstances++; 
    }
    InstanceTracker& operator=(const InstanceTracker& other) {
        value = other.value;
        return *this;
    }
    InstanceTracker& operator=(InstanceTracker&& other) noexcept {
        value = other.value;
        other.value = -1;
        return *this;
    }
    ~InstanceTracker() { activeInstances--; }
    
    bool operator==(const InstanceTracker& other) const { return value == other.value; }
};

int InstanceTracker::activeInstances = 0;

} // конец анонимного пространства имен

class LinkedListMemoryTest : public ::testing::Test {
protected:
    void SetUp() override { InstanceTracker::activeInstances = 0; }
    void TearDown() override { EXPECT_EQ(InstanceTracker::activeInstances, 0) << "CRITICAL MEMORY LEAK IN NODES DETECTED!"; }
};

// ============================================================================
// ПРАВИЛО ПЯТИ (RULE OF FIVE) И ГЛУБОКОЕ КОПИРОВАНИЕ
// ============================================================================

TEST_F(LinkedListMemoryTest, DeepCopyConstructorIsTrulyIndependent) {
    InstanceTracker arr[] = {InstanceTracker(1), InstanceTracker(2), InstanceTracker(3)};
    LinkedList<InstanceTracker> original(arr, 3);
    
    LinkedList<InstanceTracker> copy(original);
    
    EXPECT_EQ(copy.GetLength(), 3);
    EXPECT_EQ(copy.GetFirst().value, 1);
    EXPECT_EQ(copy.GetLast().value, 3);
    
    // Проверка развязки памяти: изменение копии не трогает оригинал
    copy.Append(InstanceTracker(99));
    EXPECT_EQ(original.GetLength(), 3);
    EXPECT_EQ(original.GetLast().value, 3);
    EXPECT_EQ(copy.GetLength(), 4);
    EXPECT_EQ(copy.GetLast().value, 99);
}

TEST_F(LinkedListMemoryTest, MoveSemanticsTransferOwnershipSafely) {
    InstanceTracker arr[] = {InstanceTracker(10), InstanceTracker(20)};
    LinkedList<InstanceTracker> original(arr, 2);
    
    LinkedList<InstanceTracker> moved(std::move(original)); // Вызов move-конструктора
    
    EXPECT_EQ(moved.GetLength(), 2);
    EXPECT_EQ(moved.GetLast().value, 20);
    
    // Оригинал должен быть пуст и безопасен для деструктора
    EXPECT_EQ(original.GetLength(), 0);
    EXPECT_THROW(original.GetFirst(), EmptyCollectionError);
}

TEST_F(LinkedListMemoryTest, CopyAssignmentDestroysOldData) {
    InstanceTracker arr1[] = {InstanceTracker(1), InstanceTracker(2)};
    InstanceTracker arr2[] = {InstanceTracker(3)};
    
    LinkedList<InstanceTracker> listA(arr1, 2);
    LinkedList<InstanceTracker> listB(arr2, 1);
    
    listB = listA; // listB должен удалить свой узел '3' и скопировать '1', '2'
    
    EXPECT_EQ(listB.GetLength(), 2);
    EXPECT_EQ(listB.Get(1).value, 2);
    
    // Самоприсваивание
    listB = listB;
    EXPECT_EQ(listB.GetLength(), 2);
}

TEST_F(LinkedListMemoryTest, MoveAssignmentClearsTarget) {
    InstanceTracker arr1[] = {InstanceTracker(5)};
    LinkedList<InstanceTracker> listA(arr1, 1);
    LinkedList<InstanceTracker> listB;
    listB.Append(InstanceTracker(10));
    listB.Append(InstanceTracker(20));
    
    listB = std::move(listA); // listB должен удалить 10, 20 и забрать 5
    
    EXPECT_EQ(listB.GetLength(), 1);
    EXPECT_EQ(listB.GetFirst().value, 5);
    EXPECT_EQ(listA.GetLength(), 0);
}

// ============================================================================
// ХИРУРГИЯ УКАЗАТЕЛЕЙ: HEAD, TAIL И СИНХРОНИЗАЦИЯ
// ============================================================================
class LinkedListPointersTest : public ::testing::Test {};

TEST_F(LinkedListPointersTest, EmptyListExtremes) {
    LinkedList<int> list;
    EXPECT_EQ(list.GetLength(), 0);
    EXPECT_THROW(list.GetFirst(), EmptyCollectionError);
    EXPECT_THROW(list.GetLast(), EmptyCollectionError);
    EXPECT_THROW(list.Get(0), IndexOutOfRange);
}

TEST_F(LinkedListPointersTest, AppendPrependSynchronizeHeadAndTail) {
    LinkedList<int> list;
    
    // Первый элемент (Head и Tail указывают на один узел)
    list.Append(10);
    EXPECT_EQ(list.GetFirst(), 10);
    EXPECT_EQ(list.GetLast(), 10);
    
    // Второй элемент через Prepend (Сдвигается только Head)
    list.Prepend(5);
    EXPECT_EQ(list.GetFirst(), 5);
    EXPECT_EQ(list.GetLast(), 10);
    
    // Третий элемент через Append (Сдвигается только Tail)
    list.Append(20);
    EXPECT_EQ(list.GetFirst(), 5);
    EXPECT_EQ(list.GetLast(), 20);
    EXPECT_EQ(list.GetLength(), 3);
}

TEST_F(LinkedListPointersTest, InsertAtBoundariesShiftsCorrectly) {
    LinkedList<int> list;
    
    // Insert в пустой
    list.InsertAt(1, 0);
    EXPECT_EQ(list.GetFirst(), 1);
    EXPECT_EQ(list.GetLast(), 1);
    
    // Insert в начало
    list.InsertAt(0, 0);
    EXPECT_EQ(list.GetFirst(), 0);
    
    // Insert в конец
    list.InsertAt(2, 2);
    EXPECT_EQ(list.GetLast(), 2);
    
    // Insert в середину
    list.InsertAt(99, 1);
    EXPECT_EQ(list.Get(1), 99);
    EXPECT_EQ(list.GetLength(), 4); // [0, 99, 1, 2]
    
    // Некорректные индексы
    EXPECT_THROW(list.InsertAt(100, -1), IndexOutOfRange);
    EXPECT_THROW(list.InsertAt(100, 5), IndexOutOfRange);
}

// ============================================================================
// УДАЛЕНИЕ УЗЛОВ (САМОЕ ОПАСНОЕ МЕСТО ДЛЯ ПАМЯТИ И УКАЗАТЕЛЕЙ)
// ============================================================================
TEST_F(LinkedListMemoryTest, RemoveAtUpdatesPointersAndFreesMemory) {
    InstanceTracker arr[] = {InstanceTracker(10), InstanceTracker(20), InstanceTracker(30)};
    LinkedList<InstanceTracker> list(arr, 3);
    
    // 1. Удаление из середины (Развязка next и prev соседних узлов)
    list.RemoveAt(1); // Удаляем '20'
    EXPECT_EQ(list.GetLength(), 2);
    EXPECT_EQ(list.Get(0).value, 10);
    EXPECT_EQ(list.Get(1).value, 30);
    
    // 2. Удаление начала (Обновление Head)
    list.RemoveAt(0); // Удаляем '10'
    EXPECT_EQ(list.GetLength(), 1);
    EXPECT_EQ(list.GetFirst().value, 30);
    EXPECT_EQ(list.GetLast().value, 30); // Head и Tail теперь одинаковые
    
    // 3. Удаление конца/единственного элемента (Сброс в nullptr)
    list.RemoveAt(0); // Удаляем '30'
    EXPECT_EQ(list.GetLength(), 0);
    EXPECT_THROW(list.GetFirst(), EmptyCollectionError);
    EXPECT_THROW(list.GetLast(), EmptyCollectionError);
}

TEST_F(LinkedListPointersTest, RemoveAtTailSpecifically) {
    int arr[] = {1, 2, 3};
    LinkedList<int> list(arr, 3);
    
    list.RemoveAt(2); // Удаляем хвост
    EXPECT_EQ(list.GetLast(), 2); // Tail должен сдвинуться на '2'
    EXPECT_EQ(list.GetLength(), 2);
    
    // Убеждаемся, что хвост не "грязный" и к нему можно прицепить новый узел
    list.Append(4);
    EXPECT_EQ(list.GetLast(), 4);
    EXPECT_EQ(list.Get(1), 2);
}

// ============================================================================
// ОПЕРАЦИИ ДЕКОМПОЗИЦИИ (CONCAT И GETSUBLIST)
// ============================================================================
TEST_F(LinkedListMemoryTest, GetSubListCreatesIndependentNodes) {
    InstanceTracker arr[] = {InstanceTracker(0), InstanceTracker(10), InstanceTracker(20), InstanceTracker(30)};
    LinkedList<InstanceTracker> list(arr, 4);
    
    LinkedList<InstanceTracker>* sub = list.GetSubList(1, 2); // Должен скопировать 10 и 20
    
    EXPECT_EQ(sub->GetLength(), 2);
    EXPECT_EQ(sub->GetFirst().value, 10);
    EXPECT_EQ(sub->GetLast().value, 20);
    
    // Изменение sub не должно повлиять на list
    sub->RemoveAt(0);
    EXPECT_EQ(list.Get(1).value, 10); // В оригинале '10' живо
    
    delete sub; // Обязательно убираем за собой
}

TEST_F(LinkedListMemoryTest, ConcatDirectPointerLogicIsSafe) {
    InstanceTracker arr1[] = {InstanceTracker(1)};
    InstanceTracker arr2[] = {InstanceTracker(2)};
    LinkedList<InstanceTracker> list1(arr1, 1);
    LinkedList<InstanceTracker> list2(arr2, 1);
    
    LinkedList<InstanceTracker>* combined = list1.Concat(&list2);
    
    EXPECT_EQ(combined->GetLength(), 2);
    EXPECT_EQ(combined->GetFirst().value, 1);
    EXPECT_EQ(combined->GetLast().value, 2);
    
    // Concat с nullptr
    LinkedList<InstanceTracker>* combinedNull = list1.Concat(nullptr);
    EXPECT_EQ(combinedNull->GetLength(), 1);
    
    delete combined;
    delete combinedNull;
}

// ============================================================================
// БЫСТРЫЙ ИТЕРАТОР НА УКАЗАТЕЛЯХ (O(1) НА ШАГ)
// ============================================================================
class LinkedListEnumeratorTest : public ::testing::Test {};

TEST_F(LinkedListEnumeratorTest, EmptyListIteration) {
    LinkedList<int> list;
    IEnumerator<int>* en = list.GetEnumerator();
    
    EXPECT_FALSE(en->MoveNext());
    EXPECT_THROW(en->GetCurrent(), IndexOutOfRange);
    
    delete en;
}

TEST_F(LinkedListEnumeratorTest, FastPointerTraversal) {
    int arr[] = {100, 200, 300};
    LinkedList<int> list(arr, 3);
    IEnumerator<int>* en = list.GetEnumerator();
    
    EXPECT_THROW(en->GetCurrent(), IndexOutOfRange); // До начала
    
    EXPECT_TRUE(en->MoveNext());
    EXPECT_EQ(en->GetCurrent(), 100);
    
    EXPECT_TRUE(en->MoveNext());
    EXPECT_EQ(en->GetCurrent(), 200);
    
    EXPECT_TRUE(en->MoveNext());
    EXPECT_EQ(en->GetCurrent(), 300);
    
    EXPECT_FALSE(en->MoveNext()); // Конец
    EXPECT_THROW(en->GetCurrent(), IndexOutOfRange); // Защита от мусора
    
    en->Reset();
    EXPECT_TRUE(en->MoveNext());
    EXPECT_EQ(en->GetCurrent(), 100); // Reset сработал
    
    delete en;
}

// ============================================================================
// НАГРУЗОЧНЫЕ ТЕСТЫ: ПРОВЕРКА O(1) ДЛЯ APPEND И O(N) ДЛЯ ОБХОДА
// ============================================================================
class LinkedListStressTest : public ::testing::Test {};

TEST_F(LinkedListStressTest, O1_AppendAndPrependScalability) {
    LinkedList<int> list;
    const int ELEMENTS = 500000;
    
    // Если Append/Prepend работают за O(N) из-за поиска хвоста, этот тест зависнет.
    // Если они честно работают за O(1) через указатели head/tail, он пролетит мгновенно.
    ASSERT_NO_THROW({
        for (int i = 0; i < ELEMENTS; ++i) {
            if (i % 2 == 0) list.Append(i);
            else list.Prepend(i);
        }
    });
    
    EXPECT_EQ(list.GetLength(), ELEMENTS);
}

TEST_F(LinkedListStressTest, FastEnumeratorPreventsN2) {
    LinkedList<int> list;
    const int ELEMENTS = 200000;
    for (int i = 0; i < ELEMENTS; ++i) {
        list.Append(1); // Заполняем массив
    }
    
    IEnumerator<int>* en = list.GetEnumerator();
    long long sum = 0;
    
    // Обход списка в 200k элементов через Get(i) сделал бы 20 миллиардов шагов.
    // Наш новый итератор на указателях (O(1) на шаг) сделает ровно 200k шагов.
    ASSERT_NO_THROW({
        while (en->MoveNext()) {
            sum += en->GetCurrent();
        }
    });
    
    EXPECT_EQ(sum, ELEMENTS);
    delete en;
}
