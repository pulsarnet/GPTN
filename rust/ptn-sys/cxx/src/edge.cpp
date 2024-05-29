#include <ptn/edge.h>
#include <ptn/vertex.h>

namespace vertex = ptn::net::vertex;
namespace edge = ptn::net::edge;

extern "C" {
    vertex::VertexIndex ptn$net$edge$directed$from(const edge::DirectedEdge* edge);
    vertex::VertexIndex ptn$net$edge$directed$to(const edge::DirectedEdge* edge);
    usize ptn$net$edge$directed$weight(const edge::DirectedEdge* edge);
    void ptn$net$edge$directed$set_weight(edge::DirectedEdge* edge, usize weight);

    vertex::VertexIndex ptn$net$edge$inhibitor$place(const edge::InhibitorEdge* edge);
    vertex::VertexIndex ptn$net$edge$inhibitor$transition(const edge::InhibitorEdge* edge);
}

vertex::VertexIndex edge::DirectedEdge::from() const {
    return ptn$net$edge$directed$from(this);
}

vertex::VertexIndex edge::DirectedEdge::to() const {
    return ptn$net$edge$directed$to(this);
}

usize edge::DirectedEdge::weight() const {
    return ptn$net$edge$directed$weight(this);
}

void edge::DirectedEdge::set_weight(const usize weight) {
    return ptn$net$edge$directed$set_weight(this, weight);
}

vertex::VertexIndex edge::InhibitorEdge::place() const {
    return ptn$net$edge$inhibitor$place(this);
}

vertex::VertexIndex edge::InhibitorEdge::transition() const {
    return ptn$net$edge$inhibitor$transition(this);
}