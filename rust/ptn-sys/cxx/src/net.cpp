#include <ptn/net.h>
#include <ptn/vertex.h>

namespace net = ptn::net;
namespace vertex = net::vertex;
namespace edge = net::edge;
namespace vec = ptn::alloc::vec;

extern "C" {
    net::PetriNet* ptn$net$new();
    vertex::Vertex* ptn$net$vertex(net::PetriNet*, vertex::VertexIndex);
    void ptn$net$positions(const net::PetriNet*, vec::RustVec<const vertex::Vertex*>*);
    void ptn$net$transitions(const net::PetriNet*, vec::RustVec<const vertex::Vertex*>*);
    void ptn$net$edges(const net::PetriNet*, vec::RustVec<const edge::Connection*>*);
    vertex::Vertex* ptn$net$add_position(net::PetriNet*);
    vertex::Vertex* ptn$net$insert_position(net::PetriNet*, usize, isize);
    vertex::Vertex* ptn$net$add_transition(net::PetriNet*);
    vertex::Vertex* ptn$net$insert_transition(net::PetriNet*, usize, isize);

    void ptn$net$add_edge(net::PetriNet*, vertex::VertexIndex, vertex::VertexIndex);
    void ptn$net$remove_edge(net::PetriNet*, vertex::VertexIndex, vertex::VertexIndex);

    void ptn$net$clear(net::PetriNet*);
    void ptn$net$drop(net::PetriNet*);
}

net::PetriNet* net::PetriNet::create() {
    return ptn$net$new();
}

vertex::Vertex* net::PetriNet::vertex(const vertex::VertexIndex idx) {
    return ptn$net$vertex(this, idx);
}

vec::RustVec<const vertex::Vertex*> net::PetriNet::positions() const {
    vec::RustVec<const vertex::Vertex*> vec;
    ptn$net$positions(this, &vec);
    return std::move(vec);
}

vec::RustVec<const vertex::Vertex*> net::PetriNet::transitions() const {
    vec::RustVec<const vertex::Vertex*> vec;
    ptn$net$transitions(this, &vec);
    return std::move(vec);
}

vec::RustVec<const edge::Connection*> net::PetriNet::edges() const {
    vec::RustVec<const edge::Connection*> vec;
    ptn$net$edges(this, &vec);
    return std::move(vec);
}

vertex::Vertex* net::PetriNet::add_position() {
    return ptn$net$add_position(this);
}

vertex::Vertex* net::PetriNet::insert_position(usize idx, isize parent) {
    return ptn$net$insert_position(this, idx, parent);
}

vertex::Vertex* net::PetriNet::add_transition() {
    return ptn$net$add_transition(this);
}

vertex::Vertex* net::PetriNet::insert_transition(usize idx, isize parent) {
    return ptn$net$insert_transition(this, idx, parent);
}

void net::PetriNet::add_edge(const vertex::VertexIndex from, const vertex::VertexIndex to) {
    ptn$net$add_edge(this, from, to);
}

void net::PetriNet::remove_edge(const vertex::VertexIndex from, const vertex::VertexIndex to) {
    ptn$net$remove_edge(this, from, to);
}

void net::PetriNet::clear() {
    ptn$net$clear(this);
}

void net::PetriNet::drop() {
    ptn$net$drop(this);
}