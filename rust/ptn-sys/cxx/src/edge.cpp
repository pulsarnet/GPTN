#include <ptn/edge.h>
#include <ptn/vertex.h>

namespace vertex = ptn::net::vertex;
namespace edge = ptn::net::edge;

extern "C" {
    vertex::VertexIndex ptn$net$edge$from(const edge::Connection* edge);
    vertex::VertexIndex ptn$net$edge$to(const edge::Connection* edge);
    vertex::usize ptn$net$edge$weight(const edge::Connection* edge);
    void ptn$net$edge$set_weight(edge::Connection* edge, vertex::usize weight);
}

vertex::VertexIndex edge::from(const edge::Connection* edge) {
    return ptn$net$edge$from(edge);
}

vertex::VertexIndex edge::to(const edge::Connection* edge) {
    return ptn$net$edge$from(edge);
}

vertex::usize edge::weight(const edge::Connection* edge) {
    return ptn$net$edge$weight(edge);
}

void edge::set_weight(edge::Connection* edge, vertex::usize weight) {
    return ptn$net$edge$set_weight(edge, weight);
}
