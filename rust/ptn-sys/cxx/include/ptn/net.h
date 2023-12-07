#ifndef PTN_NET
#define PTN_NET
#include <ptn/vertex.h>
#include <ptn/opaque.h>

namespace ptn::net {

    struct PetriNet final: public Opaque {
        static PetriNet* create();
        vertex::Vertex* vertex(vertex::VertexIndex);
        vertex::Vertex* add_position();
        vertex::Vertex* add_transition();

        ~PetriNet() = delete;
    };

}

#endif