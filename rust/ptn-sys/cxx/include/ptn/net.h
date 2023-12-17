#ifndef PTN_NET
#define PTN_NET
#include <ptn/vertex.h>
#include <ptn/opaque.h>
#include <ptn/vec.h>
#include <ptn/edge.h>

namespace ptn::net {

    struct PetriNet final: public Opaque {
        static PetriNet* create();
        vertex::Vertex* vertex(vertex::VertexIndex);
        [[nodiscard]] alloc::vec::RustVec<const vertex::Vertex*> positions() const;
        [[nodiscard]] alloc::vec::RustVec<const vertex::Vertex*> transitions() const;
        [[nodiscard]] alloc::vec::RustVec<const edge::Connection*> edges() const;

        vertex::Vertex* add_position();
        vertex::Vertex* insert_position(usize, isize);
        vertex::Vertex* add_transition();
        vertex::Vertex* insert_transition(usize, isize);

        // edges
        void add_edge(vertex::VertexIndex, vertex::VertexIndex);
        void remove_edge(vertex::VertexIndex, vertex::VertexIndex);

        void clear();
        void drop();

        ~PetriNet() = delete;
    };

}

#endif