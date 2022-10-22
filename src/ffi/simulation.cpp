//
// Created by darkp on 21.10.2022.
//

#include "simulation.h"

using namespace ffi;

extern "C" Simulation* create_simulation(PetriNet* net);
extern "C" int simulation_simulate(Simulation* simulation);
extern "C" int simulation_markers(Simulation* simulation, VertexIndex);
extern "C" int simulation_cycles(Simulation* simulation);
extern "C" void simulation_fired(const Simulation* simulation, CVec<VertexIndex>*);
extern "C" PetriNet* simulation_net(const Simulation* simulation);
extern "C" void drop_simulation(Simulation* simulation);

Simulation *Simulation::create(PetriNet *net) {
    return ::create_simulation(net);
}

/// Возвращает количество сработавших переходов
int Simulation::simulate() {
    return ::simulation_simulate(this);
}

int Simulation::markers(VertexIndex index) {
    return ::simulation_markers(this, index);
}

int Simulation::cycles() const {
    return ::simulation_cycles(const_cast<Simulation*>(this));
}

CVec<VertexIndex> Simulation::fired() const {
    CVec<VertexIndex> result;
    ::simulation_fired(this, &result);
    return result;
}

ffi::PetriNet *Simulation::net() const {
    return ::simulation_net(this);
}

void Simulation::destroy() {
    ::drop_simulation(this);
}
