//
// Created by nmuravev on 1/21/2022.
//

#include <memory>
#include "rust.h"

using namespace ffi;

extern "C" {
    // PetriNet
    PetriNet* create_net();
    void net_positions(PetriNet& self, CVec<Vertex*>* return$);
    void net_transitions(PetriNet& self, CVec<Vertex*>* return$);
    void net_connections(PetriNet& self, CVec<Connection*>* return$);
    Vertex* add_position(PetriNet&);
    Vertex* add_position_with(PetriNet&, usize);
    Vertex* get_position(PetriNet&, usize);
    void remove_position(PetriNet&, Vertex*);
    Vertex* add_transition(PetriNet&);
    Vertex* add_transition_with(PetriNet&, usize);
    Vertex* get_transition(PetriNet&, usize);
    void remove_transition(PetriNet&, Vertex*);
    void connect_vertexes(PetriNet&, Vertex*, Vertex*);
    void remove_connection(PetriNet&, Vertex*, Vertex*);

    // Vertex
    usize vertex_index(Vertex&);
    usize vertex_markers(Vertex&);
    void vertex_add_marker(Vertex&);
    void vertex_remove_marker(Vertex&);
    VertexType vertex_type(Vertex&);

    // Connection
    Vertex* connection_from(Connection& self);
    Vertex* connection_to(Connection& self);

    // SynthesisContext
    SynthesisContext* synthesis_init(PetriNet&);
    usize synthesis_positions(SynthesisContext&);
    usize synthesis_transitions(SynthesisContext&);
    usize synthesis_programs(SynthesisContext&);
    CMatrix* synthesis_c_matrix(SynthesisContext&);
    CMatrix* synthesis_primitive_matrix(SynthesisContext&);
    PetriNet* synthesis_primitive_net(SynthesisContext&);
    usize synthesis_position_index(SynthesisContext&, usize);
    usize synthesis_transition_index(SynthesisContext&, usize);
    PetriNet* synthesis_linear_base_fragments(SynthesisContext&);

    // CMatrix
    i32 matrix_index(CMatrix&, usize, usize);
    usize matrix_rows(CMatrix&);
    usize matrix_columns(CMatrix&);

    // CVec<u64>
    usize vec_len_u64(const CVec<usize>* self);
    const usize* vec_data_u64(const CVec<usize>* self);

    // CVec<Vertex>
    usize vec_len_vertex(const CVec<Vertex*>* self);
    Vertex* const* vec_data_vertex(const CVec<Vertex*>* self);

    // CVec<Connection>
    usize vec_len_connection(const CVec<Connection*>* self);
    Connection* const* vec_data_connection(const CVec<Connection*>* self);
};

PetriNet *PetriNet::create() {
    return ::create_net();
}

CVec<Vertex*> PetriNet::positions() {
    CVec<Vertex*> result${};
    ::net_positions(*this, &result$);
    return result$;
}

CVec<Vertex*> PetriNet::transitions() {
    CVec<Vertex*> result${};
    ::net_transitions(*this, &result$);
    return result$;
}

CVec<Connection *> PetriNet::connections() {
    CVec<Connection*> result${};
    ::net_connections(*this, &result$);
    return result$;
}

Vertex *PetriNet::add_position() {
    return ::add_position(*this);
}

Vertex *PetriNet::add_position_with(usize index) {
    return ::add_position_with(*this, index);
}

Vertex *PetriNet::get_position(usize index) {
    return ::get_position(*this, index);
}

void PetriNet::remove_position(Vertex *position) {
    return ::remove_position(*this, position);
}

Vertex *PetriNet::add_transition() {
    return ::add_transition(*this);
}

Vertex *PetriNet::add_transition_with(usize index) {
    return ::add_transition_with(*this, index);
}

Vertex *PetriNet::get_transition(usize index) {
    return ::get_transition(*this, index);
}

void PetriNet::remove_transition(Vertex *transition) {
    return ::remove_transition(*this, transition);
}

void PetriNet::connect(Vertex *from, Vertex *to) {
    return ::connect_vertexes(*this, from, to);
}

void PetriNet::remove_connection(Vertex *from, Vertex *to) {
    return ::remove_connection(*this, from, to);
}

usize Vertex::index() {
    return ::vertex_index(*this);
}

usize Vertex::markers() {
    return ::vertex_markers(*this);
}

void Vertex::add_marker() {
    ::vertex_add_marker(*this);
}

void Vertex::remove_marker() {
    ::vertex_remove_marker(*this);
}

VertexType Vertex::type() {
    return ::vertex_type(*this);
}

Vertex *Connection::from() {
    return ::connection_from(*this);
}

Vertex *Connection::to() {
    return ::connection_to(*this);
}

SynthesisContext *SynthesisContext::init(PetriNet *net) {
    return ::synthesis_init(*net);
}

usize SynthesisContext::positions() {
    return ::synthesis_positions(*this);
}

usize SynthesisContext::transitions() {
    return ::synthesis_transitions(*this);
}

usize SynthesisContext::programs() {
    return ::synthesis_programs(*this);
}

CMatrix *SynthesisContext::c_matrix() {
    return ::synthesis_c_matrix(*this);
}

CMatrix *SynthesisContext::primitive_matrix() {
    return ::synthesis_primitive_matrix(*this);
}

PetriNet *SynthesisContext::primitive_net() {
    return ::synthesis_primitive_net(*this);
}

usize SynthesisContext::position_index(usize i) {
    return ::synthesis_position_index(*this, i);
}

usize SynthesisContext::transition_index(usize i) {
    return ::synthesis_transition_index(*this, i);
}

PetriNet *SynthesisContext::linear_base_fragments() {
    return ::synthesis_linear_base_fragments(*this);
}

i32 CMatrix::index(usize row, usize col) {
    return ::matrix_index(*this, row, col);
}

usize CMatrix::rows() {
    return ::matrix_rows(*this);
}

usize CMatrix::columns() {
    return ::matrix_columns(*this);
}

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