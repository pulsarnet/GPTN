#include <ptn/simulation.h>

namespace ptn::modules::simulation {
    extern "C" {
        Simulation* ptn$modules$simulation$init(const net::PetriNet*);
        i32 ptn$modules$simulation$step(Simulation*);
        usize ptn$modules$simulation$markers_at(const Simulation*, vertex::VertexIndex);
        usize ptn$modules$simulation$cycles(const Simulation*);
        void ptn$modules$simulation$fired(const Simulation*, vec::RustVec<vertex::VertexIndex>*);
        bool ptn$modules$simulation$is_fired(const Simulation*, vertex::VertexIndex);
        net::PetriNet* ptn$modules$simulation$net(const Simulation*);
        void ptn$modules$simulation$drop(Simulation*);
    }

    Simulation* Simulation::init(const net::PetriNet* net) {
        return ptn$modules$simulation$init(net);
    }

    i32 Simulation::step() {
        return ptn$modules$simulation$step(this);
    }

    usize Simulation::markers_at(vertex::VertexIndex idx) const {
        return ptn$modules$simulation$markers_at(this, idx);
    }

    usize Simulation::cycles() const {
        return ptn$modules$simulation$cycles(this);
    }

    vec::RustVec<vertex::VertexIndex> Simulation::fired() const {
        vec::RustVec<vertex::VertexIndex> vec;
        ptn$modules$simulation$fired(this, &vec);
        return std::move(vec);
    }

    bool Simulation::is_fired(vertex::VertexIndex idx) const {
        return ptn$modules$simulation$is_fired(this, idx);
    }

    net::PetriNet* Simulation::net() const {
        return ptn$modules$simulation$net(this);
    }

    void Simulation::drop() {
        ptn$modules$simulation$drop(this);
    }
}
