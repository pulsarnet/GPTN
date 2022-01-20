//
// Created by Николай Муравьев on 10.01.2022.
//

#ifndef FFI_RUST_NET_H
#define FFI_RUST_NET_H

struct FFIPosition;
struct FFITransition;

struct PetriNet {

    static PetriNet* make();

    void connect_p(FFIPosition* p, FFITransition* t);

    void connect_t(FFITransition* t, FFIPosition* p);

    void remove_connection_p(FFIPosition* p, FFITransition* t);

    void remove_connection_t(FFITransition* t, FFIPosition* p);

    FFIPosition* add_position();

    FFIPosition* add_position_with(unsigned long index);

    FFIPosition* get_position(unsigned long index);

    void remove_position(FFIPosition* self);

    FFITransition* add_transition();

    FFITransition* add_transition_with(unsigned long index);

    FFITransition* get_transition(unsigned long index);

    void remove_transition(FFITransition* self);

    ~PetriNet();
};

#endif //FFI_RUST_NET_H
