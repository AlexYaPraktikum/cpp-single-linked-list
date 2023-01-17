#pragma once

#include <cstddef>
#include <string>
#include <utility>
#include <iostream>

#include <iterator> 

using namespace std::literals;

template <typename Type>
class SingleLinkedList
{
private:
	// Узел списка
	struct Node
	{
		Node() = default;
		Node(const Type& val, Node* next)
			: value(val)
			, next_node(next)
		{

		}
		Type value;
		Node* next_node = nullptr;
	};

	// Шаблон класса «Базовый Итератор»
	// определяет поведение итератора на элементы односвязного списка
	// ValueType — совпадает с Type (для Iterator) либо с const Type (для ConstIterator)
	template <typename ValueType>
	class BasicIterator
	{
	private:
		// Класс списка объявляется дружественным, чтобы из методов списка
		// был доступ к приватной области итератора
		friend class SingleLinkedList;
		// Конвертирующий конструктор итератора из указателя на узел списка
		explicit BasicIterator(Node* node)
		{
			node_ = node;
		}

	public:
		// Объявленные ниже типы сообщают стандартной библиотеке о свойствах этого итератора

		// Категория итератора — forward iterator
		// (итератор, который поддерживает операции инкремента и многократное разыменование)
		using iterator_category = std::forward_iterator_tag;
		// Тип элементов, по которым перемещается итератор
		using value_type = Type;
		// Тип, используемый для хранения смещения между итераторами
		using difference_type = std::ptrdiff_t;
		// Тип указателя на итерируемое значение
		using pointer = ValueType*;
		// Тип ссылки на итерируемое значение
		using reference = ValueType&;

		BasicIterator() = default;

		// Конвертирующий конструктор/конструктор копирования
		// При ValueType, совпадающем с Type, играет роль копирующего конструктора
		// При ValueType, совпадающем с const Type, играет роль конвертирующего конструктора
		BasicIterator(const BasicIterator<Type>& other) noexcept
		{
			node_ = other.node_;
		}

		// Чтобы компилятор не выдавал предупреждение об отсутствии оператора = при наличии
		// пользовательского конструктора копирования, явно объявим оператор = и
		// попросим компилятор сгенерировать его за нас
		BasicIterator& operator=(const BasicIterator& rhs) = default;

		// Оператор сравнения итераторов (в роли второго аргумента выступает константный итератор)
		// Два итератора равны, если они ссылаются на один и тот же элемент списка либо на end()
		[[nodiscard]] bool operator==(const BasicIterator<const Type>& rhs) const noexcept
		{
			return this->node_ == rhs.node_ ? 1 : 0;
		}

		// Оператор проверки итераторов на неравенство
		// Противоположен !=
		[[nodiscard]] bool operator!=(const BasicIterator<const Type>& rhs) const noexcept
		{
			return this->node_ != rhs.node_ ? 1 : 0;
		}

		// Оператор сравнения итераторов (в роли второго аргумента итератор)
		// Два итератора равны, если они ссылаются на один и тот же элемент списка либо на end()
		[[nodiscard]] bool operator==(const BasicIterator<Type>& rhs) const noexcept
		{
			return this->node_ == rhs.node_ ? 1 : 0;
		}

		// Оператор проверки итераторов на неравенство
		// Противоположен !=
		[[nodiscard]] bool operator!=(const BasicIterator<Type>& rhs) const noexcept
		{
			return this->node_ != rhs.node_ ? 1 : 0;
		}

		// Оператор прединкремента (префиксаня версия). После его вызова итератор указывает на следующий элемент списка
		// Возвращает ссылку на самого себя
		// Инкремент итератора, не указывающего на существующий элемент списка, приводит к неопределённому поведению
		BasicIterator& operator++() noexcept
		{
			node_ = node_->next_node;
			return *this;
		}

		// Оператор постинкремента (постфиксная). После его вызова итератор указывает на следующий элемент списка
		// Возвращает прежнее значение итератора
		// Инкремент итератора, не указывающего на существующий элемент списка,
		// приводит к неопределённому поведению
		BasicIterator operator++(int) noexcept
		{
			BasicIterator old_this(*this);
			++(*this);
			return old_this;
		}

		// Операция разыменования. Возвращает ссылку на текущий элемент
		// Вызов этого оператора у итератора, не указывающего на существующий элемент списка,
		// приводит к неопределённому поведению
		[[nodiscard]] reference operator*() const noexcept
		{
			return (this->node_->value);
		}

		// Операция доступа к члену класса. Возвращает указатель на текущий элемент списка
		// Вызов этого оператора у итератора, не указывающего на существующий элемент списка,
		// приводит к неопределённому поведению
		[[nodiscard]] pointer operator->() const noexcept
		{
			return &(this->node_->value);
		}

	private:
		Node* node_ = nullptr;
	};

private:
	// Фиктивный узел, используется для вставки "перед первым элементом"
	Node head_;
	size_t size_;

public:
	using value_type = Type;
	using reference = value_type&;
	using const_reference = const value_type&;
	using Iterator = BasicIterator<Type>; // Итератор, допускающий изменение элементов списка
	using ConstIterator = BasicIterator<const Type>;// Константный итератор, предоставляющий доступ для чтения к элементам списка

public:

	SingleLinkedList()
		:size_(0u)
	{

	}

	SingleLinkedList(std::initializer_list<Type> values)
	{
		SingleLinkedList temp;
		if (values.size()) //если количество переданных в initializer_list элементов больше 0
		{
			size_t size = values.size();
			auto current_value_iterator = values.end();
			for (size_t i = size; i > 0; --i) //проходим с конца контейнера values и пушим элементы в наш лист через PushFront
			{
				current_value_iterator = std::next(current_value_iterator, -1);
				temp.PushFront(*current_value_iterator);
			}
		}
		else //если количество переданных в initializer_list = 0
		{
			temp.size_ = 0;
		}
		swap(temp);
	}


	//Один из самых простых способов сделать надёжный конструктор копирования — применить идиому copy-and-swap:
	// - Создать внутри конструктора копирования временный односвязный список и последовательно скопировать внутрь него элементы исходного списка. 
	//   Если на этом этапе будет выброшено исключение, деструктор временного списка освободит память от его элементов.
	// - Когда временный список будет содержать копию исходного списка, останется использовать метод swap и обменять состояние текущего экземпляра 
	//   класса и временного списка.
	SingleLinkedList(const SingleLinkedList& other)
		:size_(0u)
	{
		//делаем проверку, что в этих переменных не лежат мусорные значения и разработчик не забыл их инициализировать при объявлении.
		//если там все-таки есть какие-то значения, то это мусор и лучше упасть с ошибкой при помощи assert.
		assert(size_ == 0 && head_.next_node == nullptr);

		SingleLinkedList temp;
		size_t size = other.GetSize();
		auto current_value_iterator = other.begin();
		for (size_t i = size; i > 0; --i) //проходим с конца контейнера values и пушим элементы в наш лист через PushFront
		{
			current_value_iterator = std::next(other.begin(), i - 1);
			temp.PushFront(*current_value_iterator);
		}
		swap(temp);
	}

	SingleLinkedList& operator=(const SingleLinkedList& rhs)
	{
		if (this != &rhs)
		{
			SingleLinkedList temp(rhs);
			swap(temp);
			return *this;
		}
		else
		{
			return *this;
		}
	}

	// Обменивает содержимое списков за время O(1)
	void swap(SingleLinkedList& other) noexcept
	{
		Node temp;
		temp.next_node = other.head_.next_node;
		size_t temp_list_size = other.size_;

		other.head_.next_node = this->head_.next_node;
		other.size_ = this->size_;

		this->head_.next_node = temp.next_node;
		this->size_ = temp_list_size;
	}

	// Возвращает итератор, ссылающийся на первый элемент
	// Если список пустой, возвращённый итератор будет равен end()
	[[nodiscard]] Iterator begin() noexcept
	{
		if (size_)
		{
			Iterator temp;
			temp.node_ = head_.next_node;
			return temp;
		}
		else
			return this->end();
	}

	// Возвращает итератор, указывающий на позицию, следующую за последним элементом односвязного списка
	// Разыменовывать этот итератор нельзя — попытка разыменования приведёт к неопределённому поведению
	[[nodiscard]] Iterator end() noexcept
	{
		size_t counter = 0;
		Node* temp_addr = head_.next_node;

		while (counter != size_)
		{
			temp_addr = temp_addr->next_node;
			counter++;
		}

		return Iterator(temp_addr);
	}

	// Возвращает константный итератор, ссылающийся на первый элемент
	// Если список пустой, возвращённый итератор будет равен end()
	// Результат вызова эквивалентен вызову метода cbegin()
	[[nodiscard]] ConstIterator begin() const noexcept
	{
		if (size_)
		{
			Iterator temp;
			temp.node_ = head_.next_node;
			return ConstIterator(temp);
		}
		else
			return this->end();
	}

	// Возвращает константный итератор, указывающий на позицию, следующую за последним элементом односвязного списка
	// Разыменовывать этот итератор нельзя — попытка разыменования приведёт к неопределённому поведению
	// Результат вызова эквивалентен вызову метода cend()
	[[nodiscard]] ConstIterator end() const noexcept
	{
		size_t counter = 0;
		Node* temp_addr = head_.next_node;

		while (counter != size_)
		{
			temp_addr = temp_addr->next_node;
			counter++;
		}

		return ConstIterator(temp_addr);
	}

	// Возвращает константный итератор, ссылающийся на первый элемент
	// Если список пустой, возвращённый итератор будет равен cend()
	[[nodiscard]] ConstIterator cbegin() const noexcept
	{
		if (size_)
		{
			Iterator temp;
			temp.node_ = head_.next_node;
			return ConstIterator(temp);
		}
		else
			return this->cend();
	}

	// Возвращает константный итератор, указывающий на позицию, следующую за последним элементом односвязного списка
	// Разыменовывать этот итератор нельзя — попытка разыменования приведёт к неопределённому поведению
	[[nodiscard]] ConstIterator cend() const noexcept
	{
		size_t counter = 0;
		Node* temp_addr = head_.next_node;

		while (counter != size_)
		{
			temp_addr = temp_addr->next_node;
			counter++;
		}

		return ConstIterator(temp_addr);
	}

	// Вставляет элемент value в начало списка за время O(1)
	void PushFront(const Type& value)
	{
		if (0 == this->GetSize())
		{
			this->head_.next_node = new Node(value, nullptr); //Это самое простое решение. К тому же оно безопасно при возникновении исключений. Если во время работы оператора new будет выброшено исключение при выделении памяти либо при конструировании объекта, объект не создастся, а память вернётся обратно в кучу. Последующее присваивание указателя head_.next_node и инкремент size_ исключений не выбрасывают.
			this->size_++;
		}
		else
		{
			this->head_.next_node = new Node(value, this->head_.next_node);
			this->size_++;
		}
	}

	// Очищает список за время O(N)
	void Clear() noexcept
	{
		while (this->head_.next_node)//Процесс очистки продолжается, пока список непустой — то есть указатель head_.next_node на первый элемент списка ненулевой.
		{
			Node* temp_copy_addr = head_.next_node->next_node;
			delete head_.next_node;

			head_.next_node = nullptr;
			head_.next_node = temp_copy_addr;

			temp_copy_addr = nullptr;
			delete temp_copy_addr;
		}
		this->size_ = 0;
	}

	// Возвращает количество элементов в списке за время O(1)
	[[nodiscard]] size_t GetSize() const noexcept
	{
		return size_;
	}

	// Сообщает, пустой ли список за время O(1)
	[[nodiscard]] bool IsEmpty() const noexcept
	{
		return size_ ? false : true;
	}

	// Возвращает итератор, указывающий на позицию перед первым элементом односвязного списка.
	// Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
	[[nodiscard]] Iterator before_begin() noexcept
	{
		Iterator temp(&head_);
		return temp;
	}

	// Возвращает константный итератор, указывающий на позицию перед первым элементом односвязного списка.
	// Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
	[[nodiscard]] ConstIterator cbefore_begin() const noexcept
	{
		ConstIterator temp(const_cast<Node*>(&head_));
		return temp;
	}

	// Возвращает константный итератор, указывающий на позицию перед первым элементом односвязного списка.
	// Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
	[[nodiscard]] ConstIterator before_begin() const noexcept
	{
		ConstIterator temp(const_cast<Node*>(&head_)); //const_cast<Node*>
		return temp;
	}

	/*
	 * Вставляет элемент value после элемента, на который указывает pos.
	 * Возвращает итератор на вставленный элемент
	 * Если при создании элемента будет выброшено исключение, список останется в прежнем состоянии
	 */
	Iterator InsertAfter(ConstIterator pos, const Type& value)
	{
		Node* new_node = new Node(value, pos.node_->next_node);
		pos.node_->next_node = new_node;

		++(this->size_);

		return Iterator(new_node);
	}

	void PopFront() noexcept
	{
		Node* temp = head_.next_node;
		this->head_.next_node = temp->next_node;
		delete temp;

		--(this->size_);
	}

	/*
	 * Удаляет элемент, следующий за pos.
	 * Возвращает итератор на элемент, следующий за удалённым
	 */
	Iterator EraseAfter(ConstIterator pos) noexcept
	{
		Node* node_to_delete = pos.node_->next_node;
		ConstIterator temp = std::next(pos);
		pos.node_->next_node = temp.node_->next_node;
		delete node_to_delete;

		--(this->size_);

		return Iterator(pos.node_->next_node);
	}

	~SingleLinkedList()
	{
		Clear();
	}

};

template <typename Type>
void swap(SingleLinkedList<Type>& lhs, SingleLinkedList<Type>& rhs) noexcept
{
	lhs.swap(rhs);
}

template <typename Type>
bool operator==(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs)
{
	if (&lhs != &rhs)
		return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
	else
		return true;
}

template <typename Type>
bool operator!=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs)
{
	if (&lhs != &rhs)
		return !(std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end()));
	else
		return false;
}

template <typename Type>
bool operator<(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
	if (&lhs != &rhs)
		return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
	else
		return false;
}

template <typename Type>
bool operator<=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs)
{
	if (&lhs != &rhs)
		return !(std::lexicographical_compare(rhs.begin(), rhs.end(), lhs.begin(), lhs.end()));
	else
		return false;
}

template <typename Type>
bool operator>(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs)
{
	if (&lhs != &rhs)
		return std::lexicographical_compare(rhs.begin(), rhs.end(), lhs.begin(), lhs.end());
	else
		return false;
}

template <typename Type>
bool operator>=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs)
{
	if (&lhs != &rhs)
		return !(std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end()));
	else
		return false;
}