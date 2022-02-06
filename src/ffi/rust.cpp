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
    char* vertex_get_name(Vertex&);
    void vertex_set_name(Vertex&, char*);
    VertexType vertex_type(Vertex&);

    // Connection
    Vertex* connection_from(Connection& self);
    Vertex* connection_to(Connection& self);

    // DecomposeContext
    DecomposeContext* decompose_context_init(PetriNet&);
    usize decompose_context_positions(DecomposeContext&);
    usize decompose_context_transitions(DecomposeContext&);
    CMatrix* decompose_context_primitive_matrix(DecomposeContext&);
    PetriNet* decompose_context_primitive_net(DecomposeContext&);
    usize decompose_context_position_index(DecomposeContext&, usize);
    usize decompose_context_transition_index(DecomposeContext&, usize);
    PetriNet* decompose_context_linear_base_fragments(DecomposeContext&);

    // SynthesisContext
    SynthesisContext* synthesis_init(DecomposeContext&);
    usize synthesis_programs(SynthesisContext&);
    void synthesis_add_program(SynthesisContext&);
    void synthesis_remove_program(SynthesisContext&, usize);
    usize synthesis_program_value(SynthesisContext&, usize, usize);
    void synthesis_set_program_value(SynthesisContext&, usize, usize, usize);
    char* synthesis_program_header_name(SynthesisContext&, usize, bool);
    CMatrix* synthesis_c_matrix(SynthesisContext&);
    PetriNet* synthesis_eval_program(SynthesisContext&, usize);
    DecomposeContext* synthesis_decompose_ctx(SynthesisContext&);

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

char *Vertex::get_name() {
    return ::vertex_get_name(*this);
}

void Vertex::set_name(char *name) {
    ::vertex_set_name(*this, name);
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

DecomposeContext *DecomposeContext::init(PetriNet *net) {
    return ::decompose_context_init(*net);
}

usize DecomposeContext::positions() {
    return ::decompose_context_positions(*this);
}

usize DecomposeContext::transitions() {
    return ::decompose_context_transitions(*this);
}

CMatrix *DecomposeContext::primitive_matrix() {
    return ::decompose_context_primitive_matrix(*this);
}

PetriNet *DecomposeContext::primitive_net() {
    return ::decompose_context_primitive_net(*this);
}

usize DecomposeContext::position_index(usize i) {
    return ::decompose_context_position_index(*this, i);
}

usize DecomposeContext::transition_index(usize i) {
    return ::decompose_context_transition_index(*this, i);
}

PetriNet *DecomposeContext::linear_base_fragments() {
    return ::decompose_context_linear_base_fragments(*this);
}

SynthesisContext *SynthesisContext::init(DecomposeContext *ctx) {
    return ::synthesis_init(*ctx);
}

usize SynthesisContext::programs() {
    return ::synthesis_programs(*this);
}

void SynthesisContext::add_program() {
    ::synthesis_add_program(*this);
}

void SynthesisContext::remove_program(usize index) {
    ::synthesis_remove_program(*this, index);
}

usize SynthesisContext::program_value(usize program, usize index) {
    return ::synthesis_program_value(*this, program, index);
}

void SynthesisContext::set_program_value(usize program, usize index, usize value) {
    ::synthesis_set_program_value(*this, program, index, value);
}

char *SynthesisContext::program_header_name(usize index, bool label) {
    return ::synthesis_program_header_name(*this, index, label);
}

CMatrix *SynthesisContext::c_matrix() {
    return ::synthesis_c_matrix(*this);
}

PetriNet *SynthesisContext::eval_program(usize index) {
    return ::synthesis_eval_program(*this, index);
}

DecomposeContext *SynthesisContext::decompose_ctx() {
    return ::synthesis_decompose_ctx(*this);
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