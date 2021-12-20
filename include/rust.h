//
// Created by nmuravev on 12/20/2021.
//

#ifndef FFI_RUST_RUST_H
#define FFI_RUST_RUST_H


extern "C" struct PetriNet;

extern "C" PetriNet* make();
extern "C" void del(PetriNet*);
extern "C" unsigned long count(PetriNet*);

extern "C" void add_position(PetriNet*, unsigned long);
extern "C" void add_transition(PetriNet*, unsigned long);
extern "C" void connect_p(PetriNet*, unsigned long, unsigned long);
extern "C" void connect_t(PetriNet*, unsigned long, unsigned long);
extern "C" void split(PetriNet*);

#endif //FFI_RUST_RUST_H
