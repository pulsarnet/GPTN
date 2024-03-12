#include <ptn/matrix.h>
#include <ptn/types.h>
#include <assert.h>

namespace matrix = ptn::matrix;

#define IMPL_FOR_TYPE_INNER(type, name) \
    extern "C" { \
        void ptn$matrix$##name##$new(matrix::RustMatrix<type>*, size_t, size_t); \
        void ptn$matrix$##name##$drop(matrix::RustMatrix<type>*); \
        size_t ptn$matrix$##name##$nrows(const matrix::RustMatrix<type>*); \
        size_t ptn$matrix$##name##$ncols(const matrix::RustMatrix<type>*); \
        matrix::RustMatrix<type>::const_pointer ptn$matrix$##name##$index(const matrix::RustMatrix<type>*, size_t, size_t); \
    } \
    \
    template<> \
    matrix::RustMatrix<type>::RustMatrix() noexcept { \
        ptn$matrix$##name##$new(this, 0, 0); \
    } \
    \
    template<> \
    matrix::RustMatrix<type>::RustMatrix(size_t n, size_t m) { \
        ptn$matrix$##name##$new(this, n, m); \
    } \
    \
    template<> \
    void matrix::RustMatrix<type>::drop() { \
        ptn$matrix$##name##$drop(this); \
    } \
    \
    template<> \
    size_t matrix::RustMatrix<type>::nrows() const noexcept { \
        return ptn$matrix$##name##$nrows(this); \
    } \
    \
    template<> \
    size_t matrix::RustMatrix<type>::ncols() const noexcept { \
        return ptn$matrix$##name##$ncols(this); \
    } \
    \
    template <> \
    typename matrix::RustMatrix<type>::const_reference matrix::RustMatrix<type>::operator[](index i) const { \
        assert(i.row < this->nrows()); \
        assert(i.col < this->ncols()); \
        return *ptn$matrix$##name##$index(this, i.row, i.col); \
    } \

#define IMPL_FOR_TYPE(type) IMPL_FOR_TYPE_INNER(type, type)

IMPL_FOR_TYPE(i32)
IMPL_FOR_TYPE(i64)