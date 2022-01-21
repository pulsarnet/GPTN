//
// Created by nmuravev on 1/21/2022.
//

#include "rust.h"

using namespace ffi;

PetriNet *PetriNet::create() {
    return ::create_net();
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

i32 CMatrix::index(usize row, usize col) {
    return ::matrix_index(*this, row, col);
}

usize CMatrix::rows() {
    return ::matrix_rows(*this);
}

usize CMatrix::columns() {
    return ::matrix_columns(*this);
}