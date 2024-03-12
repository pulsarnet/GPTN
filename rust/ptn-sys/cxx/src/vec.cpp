#include <ptn/vec.h>
#include <ptn/types.h>
#include <ptn/vertex.h>
#include <ptn/edge.h>
#include <ptn/reachability.h>

namespace vec = ptn::alloc::vec;
namespace vertex = ptn::net::vertex;
namespace edge = ptn::net::edge;
namespace reachability = ptn::modules::reachability;

#define IMPL_FOR_TYPE_INNER(type, name) \
    extern "C" { \
        void ptn$vec$##name##$new(vec::RustVec<type>*); \
        void ptn$vec$##name##$drop(vec::RustVec<type>*); \
        std::size_t ptn$vec$##name##$len(const vec::RustVec<type>*); \
        std::size_t ptn$vec$##name##$capacity(const vec::RustVec<type>*); \
        void ptn$vec$##name##$resize(vec::RustVec<type>*, std::size_t); \
        void ptn$vec$##name##$reserve(vec::RustVec<type>*, std::size_t); \
        vec::RustVec<type>::const_pointer_type ptn$vec$##name##$data(const vec::RustVec<type>*); \
    } \
    \
    template<> \
    vec::RustVec<type>::RustVec() noexcept { \
        ptn$vec$##name##$new(this); \
    } \
    \
    template<> \
    void vec::RustVec<type>::drop() { \
        ptn$vec$##name##$drop(this); \
    } \
    \
    template<> \
    std::size_t vec::RustVec<type>::size() const noexcept { \
        return ptn$vec$##name##$len(this); \
    } \
    template<> \
    std::size_t vec::RustVec<type>::capacity() const noexcept { \
        return ptn$vec$##name##$capacity(this); \
    } \
    template<> \
    void vec::RustVec<type>::resize(std::size_t size) { \
        ptn$vec$##name##$resize(this, size); \
    } \
    template<> \
    void vec::RustVec<type>::reserve(std::size_t cap) { \
        ptn$vec$##name##$reserve(this, cap); \
    } \
    template<> \
    vec::RustVec<type>::const_pointer_type vec::RustVec<type>::data() const noexcept { \
        return ptn$vec$##name##$data(this); \
    } \

#define IMPL_FOR_TYPE(type) IMPL_FOR_TYPE_INNER(type, type)

IMPL_FOR_TYPE(bool)
IMPL_FOR_TYPE(u8)
IMPL_FOR_TYPE(i8)
IMPL_FOR_TYPE(u16)
IMPL_FOR_TYPE(i16)
IMPL_FOR_TYPE(u32)
IMPL_FOR_TYPE(i32)
IMPL_FOR_TYPE(u64)
IMPL_FOR_TYPE(i64)

IMPL_FOR_TYPE_INNER(const vertex::Vertex*, const_vertex_ptr)
IMPL_FOR_TYPE_INNER(const edge::Connection*, const_edge_ptr)
IMPL_FOR_TYPE_INNER(const reachability::Marking*, const_marking_ptr)
IMPL_FOR_TYPE_INNER(vertex::VertexIndex, vertex_index)