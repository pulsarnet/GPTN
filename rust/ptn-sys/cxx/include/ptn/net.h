#ifndef PTN_NET
#define PTN_NET
#include <ptn/vertex.h>
#include <ptn/opaque.h>
#include <ptn/vec.h>
#include <ptn/edge.h>

#include "matrix.h"

namespace ptn::net {

    struct PetriNet final: public Opaque {
        static PetriNet* create();
        vertex::Vertex* vertex(vertex::VertexIndex);
        [[nodiscard]] alloc::vec::RustVec<const vertex::Vertex*> positions() const;
        [[nodiscard]] alloc::vec::RustVec<const vertex::Vertex*> transitions() const;
        [[nodiscard]] alloc::vec::RustVec<const edge::DirectedEdge*> directed_arcs() const;
        [[nodiscard]] alloc::vec::RustVec<const edge::InhibitorEdge*> inhibitor_arcs() const;

        vertex::Vertex* add_position();
        vertex::Vertex* insert_position(usize, isize);
        vertex::Vertex* add_transition();
        vertex::Vertex* insert_transition(usize, isize);
        void remove(vertex::VertexIndex);

        // edges
        [[nodiscard]] edge::DirectedEdge* directed_arc(vertex::VertexIndex, vertex::VertexIndex) const;
        void add_directed(vertex::VertexIndex, vertex::VertexIndex, uint32_t weight = 1);
        void remove_directed(vertex::VertexIndex, vertex::VertexIndex);

        [[nodiscard]] edge::InhibitorEdge* inhibitor_arc(vertex::VertexIndex, vertex::VertexIndex) const;
        void add_inhibitor(vertex::VertexIndex, vertex::VertexIndex);
        void remove_inhibitor(vertex::VertexIndex, vertex::VertexIndex);

        // representation
        [[nodiscard]] std::tuple<matrix::RustMatrix<i32>, matrix::RustMatrix<i32>> as_matrix() const;

        void clear();
        void drop();

        ~PetriNet() = delete;
    };

}

#endif