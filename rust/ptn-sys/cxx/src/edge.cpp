#include <ptn/edge.h>
#include <ptn/vertex.h>

namespace vertex = ptn::net::vertex;
namespace edge = ptn::net::edge;

extern "C" {
    vertex::VertexIndex ptn$net$edge$from(const edge::Connection* edge);
    vertex::VertexIndex ptn$net$edge$to(const edge::Connection* edge);
    usize ptn$net$edge$weight(const edge::Connection* edge);
    void ptn$net$edge$set_weight(edge::Connection* edge, usize weight);
}

vertex::VertexIndex edge::Connection::from() const {
    return ptn$net$edge$from(this);
}

vertex::VertexIndex edge::Connection::to() const {
    return ptn$net$edge$from(this);
}

usize edge::Connection::weight() const {
    return ptn$net$edge$weight(this);
}

void edge::Connection::set_weight(const usize weight) {
    return ptn$net$edge$set_weight(this, weight);
}
