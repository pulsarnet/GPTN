//
// Created by Николай Муравьев on 10.01.2022.
//

#ifndef FFI_RUST_NET_H
#define FFI_RUST_NET_H

struct FFIElement {};

extern "C" struct PetriNet;
extern "C" struct FFIPosition;
extern "C" struct FFITransition;

extern "C" PetriNet* make();
extern "C" void del(PetriNet*);
extern "C" void connect_p(PetriNet*, FFIPosition*, FFITransition*);
extern "C" void connect_t(PetriNet*, FFITransition*, FFIPosition*);
extern "C" void remove_connection_p(PetriNet*, FFIPosition*, FFITransition*);
extern "C" void remove_connection_t(PetriNet*, FFITransition*, FFIPosition*);

extern "C" FFIPosition* add_position(PetriNet*);
extern "C" FFIPosition* add_position_with(PetriNet*, unsigned long);
extern "C" FFIPosition* get_position(PetriNet*, unsigned long);
extern "C" unsigned long position_index(FFIPosition*);
extern "C" void remove_position(PetriNet*, FFIPosition*);

extern "C" FFITransition* add_transition(PetriNet*);
extern "C" FFITransition* add_transition_with(PetriNet*, unsigned long);
extern "C" FFITransition* get_transition(PetriNet*, unsigned long);
extern "C" unsigned long transition_index(FFITransition*);
extern "C" void remove_transition(PetriNet*, FFITransition*);

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

    ~PetriNet() {
        ::del(this);
    }
};

struct FFIPosition : public FFIElement {
    unsigned long index() {
        return ::position_index(this);
    }
};

struct FFITransition : public FFIElement {
    unsigned long index() {
        return ::transition_index(this);
    }
};



#endif //FFI_RUST_NET_H
