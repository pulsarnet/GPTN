#include <ptn/reachability.h>
#include <ptn/vec.h>
#include <ptn/vertex.h>
#include <ptn/types.h>

namespace reachability = ptn::modules::reachability;
namespace vec = ptn::alloc::vec;
namespace vertex = ptn::net::vertex;

extern "C" {
    void ptn$modules$reachability$positions(const reachability::ReachabilityTree*, vec::RustVec<vertex::VertexIndex>);
    void ptn$modules$reachability$marking(const reachability::ReachabilityTree*, vec::RustVec<const reachability::Marking*>);
    void ptn$modules$reachability$drop(reachability::ReachabilityTree*);

    void ptn$modules$reachability$marking$values(const reachability::Marking*, vec::RustVec<i32>);
    reachability::CovType ptn$modules$reachability$marking$type(const reachability::Marking*);
    i32 ptn$modules$reachability$marking$prev(const reachability::Marking*);
    vertex::VertexIndex ptn$modules$reachability$marking$transition(const reachability::Marking*);
}

