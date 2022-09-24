//
// Created by darkp on 24.09.2022.
//

#include "vec.h"

namespace ffi {
    struct Vertex;
    struct Connection;
    struct PetriNet;
}

using namespace ffi;

// CVec<u64>
extern "C" usize vec_len_u64(const CVec<usize>* self);
extern "C" const usize* vec_data_u64(const CVec<usize>* self);

// CVec<Vertex>
extern "C" usize vec_len_vertex(const CVec<Vertex*>* self);
extern "C" Vertex* const* vec_data_vertex(const CVec<Vertex*>* self);

// CVec<Connection>
extern "C" usize vec_len_connection(const CVec<Connection*>* self);
extern "C" Connection* const* vec_data_connection(const CVec<Connection*>* self);

// CVec<PetriNet>
extern "C" usize vec_len_nets(const CVec<PetriNet*>* self);
extern "C" PetriNet* const* vec_data_nets(const CVec<PetriNet*>* self);

template<>
usize CVec<usize>::size() const noexcept {
    return ::vec_len_u64(this);
}

template<>
const usize* CVec<usize>::data() const noexcept {
    return ::vec_data_u64(this);
}

template<>
usize CVec<Vertex*>::size() const noexcept {
    return ::vec_len_vertex(this);
}

template<>
Vertex* const* CVec<Vertex*>::data() const noexcept {
    return ::vec_data_vertex(this);
}

template<>
const std::size_t CVec<Vertex *>::size_of() const noexcept {
    return sizeof(Vertex*);
}

template<>
usize CVec<Connection*>::size() const noexcept {
    return ::vec_len_connection(this);
}

template<>
Connection* const* CVec<Connection*>::data() const noexcept {
    return ::vec_data_connection(this);
}

template<>
const std::size_t CVec<Connection *>::size_of() const noexcept {
    return sizeof(Connection*);
}

template<>
usize CVec<PetriNet*>::size() const noexcept {
    return ::vec_len_nets(this);
}

template<>
PetriNet* const* CVec<PetriNet*>::data() const noexcept {
    return ::vec_data_nets(this);
}

template<>
const std::size_t CVec<PetriNet *>::size_of() const noexcept {
    return sizeof(PetriNet*);
}
