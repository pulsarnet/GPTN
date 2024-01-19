#include <ptn/reachability.h>
#include <ptn/vec.h>
#include <ptn/vertex.h>
#include <ptn/types.h>

namespace net = ptn::net;
namespace reachability = ptn::modules::reachability;
namespace vec = ptn::alloc::vec;
namespace vertex = net::vertex;

extern "C" {
    reachability::ReachabilityTree* ptn$modules$reachability$build(const net::PetriNet*);
    void ptn$modules$reachability$positions(const reachability::ReachabilityTree*, vec::RustVec<vertex::VertexIndex>&);
    void ptn$modules$reachability$marking(const reachability::ReachabilityTree*, vec::RustVec<const reachability::Marking*>&);
    void ptn$modules$reachability$drop(reachability::ReachabilityTree*);

    void ptn$modules$reachability$marking$values(const reachability::Marking*, vec::RustVec<i32>&);
    reachability::CovType ptn$modules$reachability$marking$type(const reachability::Marking*);
    i32 ptn$modules$reachability$marking$prev(const reachability::Marking*);
    vertex::VertexIndex ptn$modules$reachability$marking$transition(const reachability::Marking*);
}

reachability::ReachabilityTree* reachability::ReachabilityTree::build(net::PetriNet* net) {
    return ptn$modules$reachability$build(net);
}

vec::RustVec<vertex::VertexIndex> reachability::ReachabilityTree::positions() const {
    vec::RustVec<vertex::VertexIndex> result{};
    ptn$modules$reachability$positions(this, result);
    return std::move(result);
}

vec::RustVec<const reachability::Marking*> reachability::ReachabilityTree::marking() const {
    vec::RustVec<const reachability::Marking*> result;
    ptn$modules$reachability$marking(this, result);
    return std::move(result);
}

void reachability::ReachabilityTree::drop() {
    ptn$modules$reachability$drop(this);
}

vec::RustVec<int> reachability::Marking::values() const {
    vec::RustVec<int> result;
    ptn$modules$reachability$marking$values(this, result);
    return result;
}

reachability::CovType reachability::Marking::type() const {
    return ptn$modules$reachability$marking$type(this);
}

vertex::VertexIndex reachability::Marking::transition() const {
    return ptn$modules$reachability$marking$transition(this);
}

i32 reachability::Marking::prev() const {
    return ptn$modules$reachability$marking$prev(this);
}


