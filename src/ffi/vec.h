#pragma once

#include <array>
#include "types.h"

namespace ffi {
    template<typename T>
    class RustRandomAccessIterator {
    public:

        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

    public:

        explicit RustRandomAccessIterator(value_type* ptr = nullptr): m_ptr(ptr) {}
        RustRandomAccessIterator(const RustRandomAccessIterator<T>& other) = default;
        ~RustRandomAccessIterator() = default;

        RustRandomAccessIterator<T>& operator=(const RustRandomAccessIterator<T>& other) = default;
        RustRandomAccessIterator<T>& operator=(T* ptr) {
            m_ptr = ptr;
            return *this;
        }

        explicit operator bool() const {
            return m_ptr;
        }

        bool operator==(const RustRandomAccessIterator<T>& other) const { return m_ptr == other.m_ptr; }
        bool operator!=(const RustRandomAccessIterator<T>& other) const { return m_ptr != other.m_ptr; }

        RustRandomAccessIterator<T>& operator+=(const difference_type move) {
            m_ptr += move;
            return *this;
        }

        RustRandomAccessIterator<T>& operator-=(const difference_type move) {
            m_ptr -= move;
            return *this;
        }

        RustRandomAccessIterator<T>& operator++() {
            ++m_ptr;
            return *this;
        }

        RustRandomAccessIterator<T>& operator--() {
            --m_ptr;
            return *this;
        }

        RustRandomAccessIterator<T> operator++(int) {
            auto tmp(*this);
            ++m_ptr;
            return tmp;
        }

        RustRandomAccessIterator<T> operator--(int) {
            auto tmp(*this);
            --m_ptr;
            return tmp;
        }

        RustRandomAccessIterator<T> operator+(const difference_type move) {
            auto save = m_ptr;
            m_ptr += move;
            auto tmp(this);
            m_ptr = save;
            return tmp;
        }

        RustRandomAccessIterator<T> operator-(const difference_type move) {
            auto save = m_ptr;
            m_ptr -= move;
            auto tmp(this);
            m_ptr = save;
            return tmp;
        }

        difference_type operator-(const RustRandomAccessIterator<T>& other) {
            return std::distance(m_ptr, other.m_ptr);
        }

        reference operator*() { return *m_ptr; }
        const reference operator*() const { return *m_ptr; };
        pointer operator->() { return m_ptr; }

    private:

        pointer m_ptr;
    };


    template<typename T>
    class CVec final {

    public:

        using value_type = T;
        using pointer_type = T*;

        typedef RustRandomAccessIterator<T>              iterator;
        typedef RustRandomAccessIterator<const T>        const_iterator;

    public:

        CVec() = default;
        CVec(const CVec<T>&) = delete;
        CVec(CVec<T>&& other) noexcept
            : m_data(other.m_data)
            , len(other.len)
            , cap(other.cap)
        {
            other.m_data = nullptr;
            other.len = 0;
            other.cap = 0;
        }

        [[nodiscard]] usize size() const noexcept;
        value_type const* data() const noexcept;
        pointer_type data() noexcept;

        const T &index(std::size_t n) const noexcept;
        const T &operator[](std::size_t n) const noexcept;
        T &operator[](std::size_t n) noexcept;

        [[nodiscard]] const std::size_t size_of() const noexcept;

        iterator begin() { return iterator(data()); }
        iterator end() { return iterator(data() + size()); }

        const_iterator cbegin() { return const_iterator(data()); }
        const_iterator cend() { return const_iterator(data() + size()); }

        ~CVec();

    private:

        pointer_type m_data = nullptr;
        size_t len = 0;
        size_t cap = 0;

    };

    template<typename T>
    size_t CVec<T>::size() const noexcept {
        return this->len;
    }

    template<typename T>
    T *CVec<T>::data() noexcept {
        return this->m_data;
    }

    template<typename T>
    const T &CVec<T>::index(std::size_t n) const noexcept {
        return this->m_data + n * size_of();
    }

    template<typename T>
    T& CVec<T>::operator[](std::size_t n) noexcept  {
        return this->m_data + n * size_of();
    }

    template<typename T>
    const T& CVec<T>::operator[](std::size_t n) const noexcept  {
        return *reinterpret_cast<const T*>(this->m_data + n * size_of());
    }
}