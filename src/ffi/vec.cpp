//
// Created by darkp on 24.09.2022.
//

#include "vec.h"

namespace ffi {
    struct Vertex;
    struct Connection;
    struct PetriNet;
}

namespace rust {
    struct Marking;
}

using namespace ffi;
using namespace rust;

extern "C" void vec_drop_i8(CVec<int8_t>);
extern "C" void vec_drop_i16(CVec<int16_t>);
extern "C" void vec_drop_i32(CVec<int32_t>);
extern "C" void vec_drop_i64(CVec<int64_t>);
extern "C" void vec_drop_u8(CVec<uint8_t>);
extern "C" void vec_drop_u16(CVec<uint16_t>);
extern "C" void vec_drop_u32(CVec<uint32_t>);
extern "C" void vec_drop_u64(CVec<uint64_t>);
extern "C" void vec_drop_Connection(CVec<Connection*>);
extern "C" void vec_drop_PetriNet(CVec<PetriNet*>);
extern "C" void vec_drop_Vertex(CVec<Vertex*>);
extern "C" void vec_drop_Marking(CVec<Marking*>);

template<>
CVec<int8_t>::~CVec() {
    vec_drop_i8(std::move(*this));
}

template<>
CVec<int16_t>::~CVec() {
    vec_drop_i16(std::move(*this));
}

template<>
CVec<int32_t>::~CVec() {
    vec_drop_i32(std::move(*this));
}

template<>
CVec<int64_t>::~CVec() {
    vec_drop_i64(std::move(*this));
}

template<>
CVec<uint8_t>::~CVec() {
    vec_drop_u8(std::move(*this));
}

template<>
CVec<uint16_t>::~CVec() {
    vec_drop_u16(std::move(*this));
}

template<>
CVec<uint32_t>::~CVec() {
    vec_drop_u32(std::move(*this));
}

template<>
CVec<uint64_t>::~CVec() {
    vec_drop_u64(std::move(*this));
}

template<>
CVec<PetriNet*>::~CVec() {
    vec_drop_PetriNet(std::move(*this));
}

template<>
CVec<Connection*>::~CVec() {
    vec_drop_Connection(std::move(*this));
}

template<>
CVec<Vertex*>::~CVec() {
    vec_drop_Vertex(std::move(*this));
}

template<>
CVec<Marking*>::~CVec() {
    vec_drop_Marking(std::move(*this));
}