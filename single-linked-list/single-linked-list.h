#pragma once

#include <cassert>
#include <string>

template <typename Type>
class SingleLinkedList {
    // Узел списка
    struct Node {
        Node() = default;
        Node(const Type& val, Node* next)
            : value(val)
            , next_node(next) {
        }
        Type value;
        Node* next_node = nullptr;
    };

    template <typename ValueType>
    class BasicIterator {
        friend class SingleLinkedList;

        // Конвертирующий конструктор итератора из указателя на узел списка
        explicit BasicIterator(Node* node)
            : node_(node)
        {
        }

    public:
        /*
			Объявленные ниже типы сообщают стандартной библиотеке о свойствах этого итератора
		*/

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
        BasicIterator(const BasicIterator<Type>& other) noexcept {
            this->node_ = other.node_;
        }

        BasicIterator& operator=(const BasicIterator& rhs) = default;

        [[nodiscard]] bool operator==(const BasicIterator<const Type>& rhs) const noexcept {
            return this->node_ == rhs.node_;
        }

        [[nodiscard]] bool operator!=(const BasicIterator<const Type>& rhs) const noexcept {
            return !(*this == rhs);
        }

        [[nodiscard]] bool operator==(const BasicIterator<Type>& rhs) const noexcept {
            return this->node_ == rhs.node_;
        }

        [[nodiscard]] bool operator!=(const BasicIterator<Type>& rhs) const noexcept {
            return !(*this == rhs);
        }

        BasicIterator& operator++() noexcept {
            assert(node_ != nullptr);
            node_ = node_->next_node;
            return *this;
        }

        BasicIterator operator++(int) noexcept {
            assert(node_ != nullptr);
            auto old_state(*this);
            ++(*this);
            return old_state;
        }

        [[nodiscard]] reference operator*() const noexcept {
            assert(node_ != nullptr);
            return node_->value;
        }

        [[nodiscard]] pointer operator->() const noexcept {
            assert(node_ != nullptr);
            return &node_->value;
        }

    private:
        Node* node_ = nullptr;
    };

public:
    using value_type = Type;
    using reference = value_type&;
    using const_reference = const value_type&;

    // Итератор, допускающий изменение элементов списка
    using Iterator = BasicIterator<Type>;
    // Константный итератор, предоставляющий доступ для чтения к элементам списка
    using ConstIterator = BasicIterator<const Type>;

    [[nodiscard]] Iterator begin() noexcept {
        return Iterator(head_.next_node);
    }

    [[nodiscard]] Iterator end() noexcept {
        return Iterator(nullptr);
    }

    [[nodiscard]] ConstIterator begin() const noexcept {
        return ConstIterator(head_.next_node);
    }

    [[nodiscard]] ConstIterator end() const noexcept {
        return ConstIterator(nullptr);
    }

    [[nodiscard]] ConstIterator cbegin() const noexcept {
        return begin();
    }

    [[nodiscard]] ConstIterator cend() const noexcept {
        return end();
    }

    [[nodiscard]] Iterator before_begin() noexcept {
        return Iterator(&head_);
    }

    [[nodiscard]] ConstIterator before_begin() const noexcept {
        return ConstIterator(const_cast<Node*>(&head_));
    }

    [[nodiscard]] ConstIterator cbefore_begin() const noexcept {
        return before_begin();
    }

    SingleLinkedList()
        : size_ (0)
    {
    };

    SingleLinkedList(std::initializer_list<Type> values) {
        SingleLinkedList tmp;
        tmp.PushRange(values.begin(), values.end());
        swap(tmp);
    }

    // Конструктор копирования
    SingleLinkedList(const SingleLinkedList& other) {
        SingleLinkedList tmp;
        tmp.PushRange(other.begin(), other.end());
        swap(tmp);
    }

    SingleLinkedList& operator=(const SingleLinkedList& rhs) {
        if (this != &rhs) {
            SingleLinkedList tmp(rhs);
            swap(tmp);
        }
        return *this;
    }
   
    void swap(SingleLinkedList& other) noexcept {
        std::swap(this->head_.next_node, other.head_.next_node);
        std::swap(this->size_, other.size_);
    }

    // Возвращает количество элементов в списке
    [[nodiscard]] size_t GetSize() const noexcept {
        return size_;
    }

    // Сообщает, пустой ли список
    [[nodiscard]] bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    // Вставляет элемент value в начало списка за время O(1)
    void PushFront(const Type& value) {
        head_.next_node = new Node(value, head_.next_node);
        ++size_;
    }

    // Рекурсивно заполняет контейнер диапазоном значений
    template <typename IteratopType>
    void PushRange(const IteratopType& range_begin, const IteratopType& range_end) {
        if (range_begin == range_end) {
            return;
        }
        Node* ptr = &head_;
        for (auto it = range_begin; it != range_end; ++it) {
            ptr->next_node = new Node(*it, nullptr);
            ptr = ptr->next_node;
            ++size_;
        }
        ptr = nullptr;
    }


    // Вставляет элемент value после элемента, на который указывает pos.
    // Возвращает итератор на вставленный элемент
    // Если при создании элемента будет выброшено исключение, список останется в прежнем состоянии
    Iterator InsertAfter(ConstIterator pos, const Type& value) {
        assert(pos.node_ != nullptr);
        auto ptr = new Node(value, nullptr);
        ptr->next_node = pos.node_->next_node;
        pos.node_->next_node = ptr;
        ++size_;
        return Iterator(ptr);
    }

	// Удаляет элемент в начале списка
    void PopFront() noexcept {
        if (size_ == 0) {
            return;
        }
        auto ptr = head_.next_node;
        head_.next_node = ptr->next_node;
        delete ptr;
        --size_;
    }

    // Удаляет элемент, следующий за pos.
    // Возвращает итератор на элемент, следующий за удалённым
    Iterator EraseAfter(ConstIterator pos) noexcept {
        assert(pos.node_ != nullptr);
        auto delete_ptr = pos.node_->next_node;
        pos.node_->next_node = delete_ptr->next_node;
        delete delete_ptr;
        --size_;
        return Iterator(pos.node_->next_node);
    }

    // Очищает список за время O(N)
    void Clear() noexcept {
        while (head_.next_node) {
            Node* first_node = head_.next_node;
            head_.next_node = first_node->next_node;
            delete first_node;
        }
        size_ = 0;
    }

    ~SingleLinkedList() {
        Clear();
    }

private:
    // Фиктивный узел, используется для вставки "перед первым элементом"
    Node head_;
    size_t size_;
};

template <typename Type>
void swap(SingleLinkedList<Type>& lhs, SingleLinkedList<Type>& rhs) noexcept {
    lhs.swap(rhs);
}

template <typename Type>
bool operator==(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
}

template <typename Type>
bool operator!=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
bool operator<(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return std::lexicographical_compare(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
}

template <typename Type>
bool operator<=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(rhs < lhs);
}

template <typename Type>
bool operator>(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return rhs < lhs;
}

template <typename Type>
bool operator>=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs < rhs);
}