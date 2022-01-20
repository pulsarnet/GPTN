//
// Created by Николай Муравьев on 10.01.2022.
//

#include "net.h"
#include "methods.h"

PetriNet *PetriNet::make() {
    return ::make();
}

void PetriNet::connect_p(FFIPosition *p, FFITransition *t) {
    ::connect_p(this, p, t);
}

void PetriNet::connect_t(FFITransition *t, FFIPosition *p) {
    ::connect_t(this, t, p);
}

void PetriNet::remove_connection_p(FFIPosition *p, FFITransition *t) {
    ::remove_connection_p(this, p, t);
}

void PetriNet::remove_connection_t(FFITransition *t, FFIPosition *p) {
    ::remove_connection_t(this, t, p);
}

FFIPosition *PetriNet::add_position() {
    return ::add_position(this);
}

FFIPosition *PetriNet::add_position_with(unsigned long index) {
    return ::add_position_with(this, index);
}

FFIPosition *PetriNet::get_position(unsigned long index) {
    return ::get_position(this, index);
}

void PetriNet::remove_position(FFIPosition *self) {
    return ::remove_position(this, self);
}

FFITransition *PetriNet::add_transition() {
    return ::add_transition(this);
}

FFITransition *PetriNet::add_transition_with(unsigned long index) {
    return ::add_transition_with(this, index);
}

FFITransition *PetriNet::get_transition(unsigned long index) {
    return ::get_transition(this, index);
}

void PetriNet::remove_transition(FFITransition *self) {
    return ::remove_transition(this, self);
}

PetriNet::~PetriNet() {
    ::del(this);
}
