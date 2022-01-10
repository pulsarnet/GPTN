//
// Created by Николай Муравьев on 10.01.2022.
//

#ifndef FFI_RUST_METHODS_H
#define FFI_RUST_METHODS_H

extern "C" struct PetriNet;

extern "C" struct FFIPosition {
    unsigned long index();

    unsigned long markers();

    void add_marker();

    void remove_marker();

    void set_markers(unsigned long markers);

};

extern "C" struct FFITransition {
    unsigned long index();
};

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
extern "C" unsigned long markers(FFIPosition*);
extern "C" void add_marker(FFIPosition*);
extern "C" void remove_marker(FFIPosition*);
extern "C" void set_markers(FFIPosition*, unsigned long);

extern "C" FFITransition* add_transition(PetriNet*);
extern "C" FFITransition* add_transition_with(PetriNet*, unsigned long);
extern "C" FFITransition* get_transition(PetriNet*, unsigned long);
extern "C" unsigned long transition_index(FFITransition*);
extern "C" void remove_transition(PetriNet*, FFITransition*);


#endif //FFI_RUST_METHODS_H
