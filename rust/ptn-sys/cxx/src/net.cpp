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
    void ptn$net$directed$arcs(const net::PetriNet*, vec::RustVec<const edge::DirectedEdge*>*);
    void ptn$net$inhibitor$arcs(const net::PetriNet*, vec::RustVec<const edge::InhibitorEdge*>*);

    vertex::Vertex* ptn$net$add_position(net::PetriNet*);
    vertex::Vertex* ptn$net$insert_position(net::PetriNet*, usize, isize);
    vertex::Vertex* ptn$net$add_transition(net::PetriNet*);
    vertex::Vertex* ptn$net$insert_transition(net::PetriNet*, usize, isize);
    void ptn$net$remove(net::PetriNet*, vertex::VertexIndex);

    edge::DirectedEdge* ptn$net$directed$arc(const net::PetriNet*, vertex::VertexIndex, vertex::VertexIndex);
    void ptn$net$add_directed(net::PetriNet*, vertex::VertexIndex, vertex::VertexIndex, uint32_t);
    void ptn$net$remove_directed(net::PetriNet*, vertex::VertexIndex, vertex::VertexIndex);

    edge::InhibitorEdge* ptn$net$inhibitor$arc(const net::PetriNet*, vertex::VertexIndex, vertex::VertexIndex);
    void ptn$net$add_inhibitor(net::PetriNet*, vertex::VertexIndex, vertex::VertexIndex);
    void ptn$net$remove_inhibitor(net::PetriNet*, vertex::VertexIndex, vertex::VertexIndex);

    void ptn$net$as_matrix(const net::PetriNet*, ptn::matrix::RustMatrix<i32>&, ptn::matrix::RustMatrix<i32>&);

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

vec::RustVec<const edge::DirectedEdge*> net::PetriNet::directed_arcs() const {
    vec::RustVec<const edge::DirectedEdge*> vec;
    ptn$net$directed$arcs(this, &vec);
    return std::move(vec);
}

vec::RustVec<const edge::InhibitorEdge*> net::PetriNet::inhibitor_arcs() const {
    vec::RustVec<const edge::InhibitorEdge*> vec;
    ptn$net$inhibitor$arcs(this, &vec);
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

void net::PetriNet::remove(const vertex::VertexIndex index) {
    ptn$net$remove(this, index);
}

edge::DirectedEdge* net::PetriNet::directed_arc(const vertex::VertexIndex from, const vertex::VertexIndex to) const {
    return ptn$net$directed$arc(this, from, to);
}

void net::PetriNet::add_directed(const vertex::VertexIndex from, const vertex::VertexIndex to, uint32_t weight) {
    ptn$net$add_directed(this, from, to, weight);
}

void net::PetriNet::remove_directed(const vertex::VertexIndex from, const vertex::VertexIndex to) {
    ptn$net$remove_directed(this, from, to);
}

edge::InhibitorEdge* net::PetriNet::inhibitor_arc(const vertex::VertexIndex from, const vertex::VertexIndex to) const {
    return ptn$net$inhibitor$arc(this, from, to);
}

void net::PetriNet::add_inhibitor(const vertex::VertexIndex from, const vertex::VertexIndex to) {
    ptn$net$add_inhibitor(this, from, to);
}

void net::PetriNet::remove_inhibitor(const vertex::VertexIndex from, const vertex::VertexIndex to) {
    ptn$net$remove_directed(this, from, to);
}


std::tuple<ptn::matrix::RustMatrix<i32>, ptn::matrix::RustMatrix<i32>> net::PetriNet::as_matrix() const {
    matrix::RustMatrix<i32> input;
    matrix::RustMatrix<i32> output;
    ptn$net$as_matrix(this, input, output);
    return std::make_tuple(std::move(input), std::move(output));
}

void net::PetriNet::clear() {
    ptn$net$clear(this);
}

void net::PetriNet::drop() {
    ptn$net$drop(this);
}