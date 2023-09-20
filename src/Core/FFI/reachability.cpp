//
// Created by darkp on 26.09.2022.
//

#include "reachability.h"

using namespace rust;
using namespace ffi;

extern "C" void reachability_positions(const ReachabilityTree&, CVec<VertexIndex>&);
extern "C" void reachability_marking(const ReachabilityTree&, CVec<Marking*>&);
extern "C" void reachability_drop(ReachabilityTree*);

extern "C" void marking_values(const Marking&,  CVec<int32_t>&);
extern "C" int32_t marking_previous(const Marking&);
extern "C" ffi::VertexIndex marking_transition(const Marking&);

ffi::CVec<int32_t> Marking::values() const {
    CVec<int32_t> values{};
    ::marking_values(*this, values);
    return values;
}

int32_t Marking::prev() const {
    return ::marking_previous(*this);
}

ffi::VertexIndex Marking::transition() const {
    return ::marking_transition(*this);
}

ffi::CVec<VertexIndex> ReachabilityTree::indexes() const {
    CVec<VertexIndex> indexes{};
    ::reachability_positions(*this, indexes);
    return indexes;
}

CVec<Marking *> ReachabilityTree::marking() const {
    CVec<Marking*> marking{};
    ::reachability_marking(*this, marking);
    return marking;
}

ReachabilityTree::~ReachabilityTree() {
    ::reachability_drop(this);
}
