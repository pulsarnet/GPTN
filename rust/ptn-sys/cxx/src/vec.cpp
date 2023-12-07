#include <ptn/vec.h>
#include <ptn/types.h>

namespace vec = ptn::alloc::vec;

#define IMPL_FOR_TYPE(type) \
    extern "C" { \
        void ptn$vec$##type##$new(vec::RustVec<type>*); \
        void ptn$vec$##type##$drop(vec::RustVec<type>*); \
        std::size_t ptn$vec$##type##$len(const vec::RustVec<type>*); \
        std::size_t ptn$vec$##type##$capacity(const vec::RustVec<type>*); \
        void ptn$vec$##type##$resize(vec::RustVec<type>*, std::size_t); \
        void ptn$vec$##type##$reserve(vec::RustVec<type>*, std::size_t); \
        vec::RustVec<type>::const_pointer_type ptn$vec$##type##$data(const vec::RustVec<type>*); \
    } \
    \
    template<> \
    vec::RustVec<type>::RustVec() noexcept { \
        ptn$vec$##type##$new(this); \
    } \
    \
    template<> \
    void vec::RustVec<type>::drop() { \
        ptn$vec$##type##$drop(this); \
    } \
    \
    template<> \
    std::size_t vec::RustVec<type>::size() const noexcept { \
        return ptn$vec$##type##$len(this); \
    } \
    template<> \
    std::size_t vec::RustVec<type>::capacity() const noexcept { \
        return ptn$vec$##type##$capacity(this); \
    } \
    template<> \
    void vec::RustVec<type>::resize(std::size_t size) { \
        ptn$vec$##type##$resize(this, size); \
    } \
    template<> \
    void vec::RustVec<type>::reserve(std::size_t cap) { \
        ptn$vec$##type##$reserve(this, cap); \
    } \
    template<> \
    vec::RustVec<type>::const_pointer_type vec::RustVec<type>::data() const noexcept { \
        return ptn$vec$##type##$data(this); \
    } \


IMPL_FOR_TYPE(bool)
IMPL_FOR_TYPE(u8)
IMPL_FOR_TYPE(i8)
IMPL_FOR_TYPE(u16)
IMPL_FOR_TYPE(i16)
IMPL_FOR_TYPE(u32)
IMPL_FOR_TYPE(i32)
IMPL_FOR_TYPE(u64)
IMPL_FOR_TYPE(i64)