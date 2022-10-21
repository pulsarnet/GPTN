//
// Created by darkp on 21.10.2022.
//

#include "simulation.h"

using namespace ffi;

extern "C" Simulation* create_simulation(PetriNet* net);
extern "C" void simulation_simulate(Simulation* simulation);
extern "C" int simulation_markers(Simulation* simulation, VertexIndex);
extern "C" void drop_simulation(Simulation* simulation);

Simulation *Simulation::create(PetriNet *net) {
    return ::create_simulation(net);
}

void Simulation::simulate() {
    ::simulation_simulate(this);
}

int Simulation::markers(VertexIndex index) {
    return ::simulation_markers(this, index);
}

void Simulation::destroy() {
    ::drop_simulation(this);
}