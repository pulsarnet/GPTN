//
// Created by Николай Муравьев on 10.01.2022.
//

#ifndef FFI_RUST_NET_H
#define FFI_RUST_NET_H

#include "methods.h"
#include "../elements/petri_object.h"

struct PetriNet {

    static PetriNet* make() {
        return ::make();
    }

    void connect_p(FFIPosition* p, FFITransition* t) {
        ::connect_p(this, p, t);
    }

    void connect_t(FFITransition* t, FFIPosition* p) {
        ::connect_t(this, t, p);
    }

    void remove_connection_p(FFIPosition* p, FFITransition* t) {
        ::remove_connection_p(this, p, t);
    }

    void remove_connection_t(FFITransition* t, FFIPosition* p) {
        ::remove_connection_t(this, t, p);
    }

    FFIPosition* add_position() {
        return ::add_position(this);
    }

    FFIPosition* add_position_with(unsigned long index) {
        return ::add_position_with(this, index);
    }

    FFIPosition* get_position(unsigned long index) {
        return ::get_position(this, index);
    }

    void remove_position(FFIPosition* self) {
        return ::remove_position(this, self);
    }

    FFITransition* add_transition() {
        return ::add_transition(this);
    }

    FFITransition* add_transition_with(unsigned long index) {
        return ::add_transition_with(this, index);
    }

    FFITransition* get_transition(unsigned long index) {
        return ::get_transition(this, index);
    }

    void remove_transition(FFITransition* self) {
        return ::remove_transition(this, self);
    }

    void remove_object(PetriObject* object);

    ~PetriNet() {
        ::del(this);
    }
};

#endif //FFI_RUST_NET_H
