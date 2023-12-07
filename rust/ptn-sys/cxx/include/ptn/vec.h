#ifndef PTN_VEC_H
#define PTN_VEC_H

#include <algorithm>
#include <array>
#include <iterator>

namespace ptn::alloc::vec {
    template<typename T>
    class RustRandomAccessIterator {
    public:

        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        explicit RustRandomAccessIterator(value_type* ptr = nullptr): m_ptr(ptr) {}
        RustRandomAccessIterator(const RustRandomAccessIterator<T>& other) = default;
        ~RustRandomAccessIterator() = default;

        RustRandomAccessIterator& operator=(const RustRandomAccessIterator& other) = default;
        RustRandomAccessIterator& operator=(T* ptr) {
            m_ptr = ptr;
            return *this;
        }

        explicit operator bool() const {
            return m_ptr;
        }

        bool operator==(const RustRandomAccessIterator& other) const { return m_ptr == other.m_ptr; }
        bool operator!=(const RustRandomAccessIterator& other) const { return m_ptr != other.m_ptr; }

        RustRandomAccessIterator& operator+=(const difference_type move) {
            m_ptr += move;
            return *this;
        }

        RustRandomAccessIterator& operator-=(const difference_type move) {
            m_ptr -= move;
            return *this;
        }

        RustRandomAccessIterator& operator++() {
            ++m_ptr;
            return *this;
        }

        RustRandomAccessIterator& operator--() {
            --m_ptr;
            return *this;
        }

        RustRandomAccessIterator operator++(int) {
            auto tmp(*this);
            ++m_ptr;
            return tmp;
        }

        RustRandomAccessIterator operator--(int) {
            auto tmp(*this);
            --m_ptr;
            return tmp;
        }

        RustRandomAccessIterator operator+(const difference_type move) {
            auto save = m_ptr;
            m_ptr += move;
            auto tmp(this);
            m_ptr = save;
            return tmp;
        }

        RustRandomAccessIterator operator-(const difference_type move) {
            auto save = m_ptr;
            m_ptr -= move;
            auto tmp(this);
            m_ptr = save;
            return tmp;
        }

        difference_type operator-(const RustRandomAccessIterator& other) {
            return std::distance(m_ptr, other.m_ptr);
        }

        reference operator*() { return *m_ptr; }
        const reference operator*() const { return *m_ptr; };
        pointer operator->() { return m_ptr; }

    private:

        pointer m_ptr;
    };


    template<typename T>
    class RustVec final {

    public:

        using value_type = T;
        using pointer_type = T*;
        using const_pointer_type = const T*;

        typedef RustRandomAccessIterator<T>              iterator;
        typedef RustRandomAccessIterator<const T>        const_iterator;

        RustVec() noexcept;
        RustVec(std::initializer_list<T>);
        RustVec(const RustVec &);
        RustVec(RustVec &&) noexcept;

        RustVec &operator=(RustVec&&) &noexcept;
        RustVec &operator=(const RustVec&) &;

        [[nodiscard]] bool empty() const noexcept;
        [[nodiscard]] std::size_t size() const noexcept;
        [[nodiscard]] std::size_t capacity() const noexcept;

        void reserve(std::size_t cap);
        void resize(std::size_t size);

        const_pointer_type data() const noexcept;
        pointer_type data() noexcept;

        const T &index(std::size_t n) const noexcept;
        const T &operator[](std::size_t n) const noexcept;
        T &operator[](std::size_t n) noexcept;

        iterator begin() { return iterator(data()); }
        iterator end() { return iterator(data() + size()); }

        const_iterator cbegin() { return const_iterator(data()); }
        const_iterator cend() { return const_iterator(data() + size()); }

        ~RustVec();

    private:

        void drop();

        std::array<std::uintptr_t, 3> repr;
    };


    template<typename T>
    RustVec<T>::RustVec(std::initializer_list<T> list) {
        this->reserve(list.size());
        std::move(list.begin(), list.end(), std::back_inserter(*this));
    }

    template<typename T>
    RustVec<T>::RustVec(const RustVec& other) {
        this->reserve(other.size());
        std::copy(other.begin(), other.end(), std::back_inserter(*this));
    }

    template<typename T>
    RustVec<T>::RustVec(RustVec&& other) noexcept : repr(other.repr) {
        new (&other) RustVec();
    }

    template<typename T>
    RustVec<T>::~RustVec() {
        this->drop();
    }

    template<typename T>
    RustVec<T> &RustVec<T>::operator=(RustVec&& other) &noexcept {
        this->drop();
        this->repr = other.repr;
        new (&other) RustVec();
        return *this;
    }

    template<typename T>
    RustVec<T> &RustVec<T>::operator=(const RustVec& other) & {
        if (this != other) {
            this->drop();
            new (this) RustVec(other);
        }
        return *this;
    }

    template<typename T>
    bool RustVec<T>::empty() const noexcept {
        return this->size() == 0;
    }

    template<typename T>
    typename RustVec<T>::pointer_type RustVec<T>::data() noexcept {
        return const_cast<T *>(const_cast<const RustVec *>(this)->data());
    }

    template<typename T>
    const T& RustVec<T>::operator[](std::size_t n) const noexcept {
        assert(n < this->size());
        auto data = reinterpret_cast<const char *>(this->data());
        // todo layout sizeof
        return *reinterpret_cast<const T*>(data + n * sizeof(T));
    }

}

#endif
