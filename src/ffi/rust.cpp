//
// Created by nmuravev on 1/21/2022.
//

#include <memory>
#include "rust.h"

using namespace ffi;

extern "C" {
    // PetriNet
    PetriNet* create_net();
    void net_positions(PetriNet& self, CVec<Position>* return$);
    void net_transitions(PetriNet& self, CVec<Transition>* return$);
    Position* add_position(PetriNet&);
    Position* add_position_with(PetriNet&, usize);
    Position* get_position(PetriNet&, usize);
    void remove_position(PetriNet&, Position&);
    Transition* add_transition(PetriNet&);
    Transition* add_transition_with(PetriNet&, usize);
    Transition* get_transition(PetriNet&, usize);
    void remove_transition(PetriNet&, Transition&);
    void connect_p(PetriNet&, Position&, Transition&);
    void connect_t(PetriNet&, Transition&, Position&);
    void remove_connection_p(PetriNet&, Position&, Transition&);
    void remove_connection_t(PetriNet&, Transition&, Position&);

    // Position
    usize position_index(Position&);
    usize position_markers(Position&);
    void position_add_marker(Position&);
    void position_remove_marker(Position&);

    // Transition
    usize transition_index(Transition&);

    // SynthesisContext
    SynthesisContext* synthesis_init(PetriNet&);
    usize synthesis_positions(SynthesisContext&);
    usize synthesis_transitions(SynthesisContext&);
    usize synthesis_programs(SynthesisContext&);
    CMatrix* synthesis_c_matrix(SynthesisContext&);
    CMatrix* synthesis_primitive_matrix(SynthesisContext&);
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

    // CVec<Position>
    usize vec_len_position(const CVec<Position>* self);
    const Position* vec_data_position(const CVec<Position>* self);

    // CVec<Transition>
    usize vec_len_transition(const CVec<Transition>* self);
    const Transition* vec_data_transition(const CVec<Transition>* self);

    // size_of
    usize size_of_position();
    usize size_of_transition();
};

PetriNet *PetriNet::create() {
    return ::create_net();
}

CVec<Position> PetriNet::positions() {
    CVec<Position> result$;
    ::net_positions(*this, &result$);
    return result$;
}

CVec<Transition> PetriNet::transitions() {
    CVec<Transition> result$;
    ::net_transitions(*this, &result$);
    return result$;
}

Position *PetriNet::add_position() {
    return ::add_position(*this);
}

Position *PetriNet::add_position_with(usize index) {
    return ::add_position_with(*this, index);
}

Position *PetriNet::get_position(usize index) {
    return ::get_position(*this, index);
}

void PetriNet::remove_position(Position *position) {
    return ::remove_position(*this, *position);
}

Transition *PetriNet::add_transition() {
    return ::add_transition(*this);
}

Transition *PetriNet::add_transition_with(usize index) {
    return ::add_transition_with(*this, index);
}

Transition *PetriNet::get_transition(usize index) {
    return ::get_transition(*this, index);
}

void PetriNet::remove_transition(Transition *transition) {
    return ::remove_transition(*this, *transition);
}

void PetriNet::connect_p(Position *position, Transition *transition) {
    return ::connect_p(*this, *position, *transition);
}

void PetriNet::connect_t(Transition *transition, Position *position) {
    return ::connect_t(*this, *transition, *position);
}

void PetriNet::remove_connection_p(Position *position, Transition *transition) {
    return ::remove_connection_p(*this, *position, *transition);
}

void PetriNet::remove_connection_t(Transition *transition, Position *position) {
    return ::remove_connection_t(*this, *transition, *position);
}

usize Position::index() {
    return ::position_index(*this);
}

usize Position::markers() {
    return ::position_markers(*this);
}

void Position::add_marker() {
    ::position_add_marker(*this);
}

void Position::remove_marker() {
    ::position_remove_marker(*this);
}

usize Transition::index() {
    return ::transition_index(*this);
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
const usize *CVec<usize>::data() const noexcept {
    return ::vec_data_u64(this);
}

template<>
usize CVec<Position>::size() const noexcept {
    return ::vec_len_position(this);
}

template<>
const Position *CVec<Position>::data() const noexcept {
    return ::vec_data_position(this);
}

template<>
const std::size_t CVec<Position>::size_of() const noexcept {
    return ::size_of_position();
}

template<>
usize CVec<Transition>::size() const noexcept {
    return ::vec_len_transition(this);
}

template<>
const Transition *CVec<Transition>::data() const noexcept {
    return ::vec_data_transition(this);
}

template<>
const std::size_t CVec<Transition>::size_of() const noexcept {
    return ::size_of_transition();
}

