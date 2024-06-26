//
// Created by darkp on 21.10.2022.
//

#ifndef FFI_RUST_SIMULATION_H
#define FFI_RUST_SIMULATION_H

#include "rust.h"

namespace ffi {
    struct UpdateMarking {
        VertexIndex vertex;
        usize marking;
    };

    struct FiredTransition {
        VertexIndex transition;  // transition: VertexIndex,
        CVec<UpdateMarking> input;  // input: CVec<UpdateMarking>,
        CVec<UpdateMarking> output; // output: CVec<UpdateMarking>,
    };

    struct Simulation {
        static Simulation* create(PetriNet* net);
        int simulate();
        int markers(VertexIndex index);
        bool isFired(VertexIndex index) const;
        int cycles() const;
        CVec<VertexIndex> fired() const;
        ffi::PetriNet* net() const;
        void destroy();
    };
}


#endif //FFI_RUST_SIMULATION_H
