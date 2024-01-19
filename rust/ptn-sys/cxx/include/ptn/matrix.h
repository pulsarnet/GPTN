#ifndef PTN_MATRIX_H
#define PTN_MATRIX_H

#include <array>
#include <cstdint>

namespace ptn::matrix {

    template<typename T>
    class RustMatrix final {

    public:

        using value_type = T;
        using pointer = T*;
        using const_pointer = const T*;
        using reference = T&;
        using const_reference = const T&;

        RustMatrix() noexcept;
        RustMatrix(std::size_t n, std::size_t m);
        RustMatrix(const RustMatrix&) = delete;
        RustMatrix(RustMatrix&&) noexcept;

        [[nodiscard]] std::size_t nrows() const noexcept;
        [[nodiscard]] std::size_t ncols() const noexcept;

        struct index { std::size_t row; std::size_t col; };
        reference operator[](index i);
        const_reference operator[](index i) const;

        ~RustMatrix();

    protected:

        void drop();

    private:

        std::array<std::uintptr_t, 5> repr;
    };

    template <typename T>
    RustMatrix<T>::RustMatrix(RustMatrix&& other) noexcept : repr(other.repr) {
        new (&other) RustMatrix();
    }

    template <typename T>
    typename RustMatrix<T>::reference RustMatrix<T>::operator[](index i) {
        return const_cast<reference>(const_cast<const RustMatrix*>(this)->operator[](i));
    }

    template<typename T>
    RustMatrix<T>::~RustMatrix() {
        this->drop();
    }

}

#endif //MATRIX_H