#include <ptn/net.h>
#include <ptn/vertex.h>

namespace net = ptn::net;
namespace vertex = net::vertex;

extern "C" {
    net::PetriNet* ptn$net$new();
    vertex::Vertex* ptn$net$vertex(net::PetriNet*, vertex::VertexIndex);
    vertex::Vertex* ptn$net$add_position(net::PetriNet*);
    vertex::Vertex* ptn$net$add_transition(net::PetriNet*);
}

net::PetriNet* net::PetriNet::create() {
    return ptn$net$new();
}

vertex::Vertex* net::PetriNet::vertex(const vertex::VertexIndex idx) {
    return ptn$net$vertex(this, idx);
}

vertex::Vertex* net::PetriNet::add_position() {
    return ptn$net$add_position(this);
}

vertex::Vertex* net::PetriNet::add_transition() {
    return ptn$net$add_transition(this);
}
