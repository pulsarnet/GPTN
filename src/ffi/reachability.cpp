//
// Created by darkp on 26.09.2022.
//

#include "reachability.h"

using namespace rust;
using namespace ffi;

extern "C" void reachability_marking(const Reachability&, CVec<Marking*>&);
extern "C" void reachability_drop(Reachability*);

extern "C" void marking_values(const Marking&,  CVec<int32_t>&);
extern "C" int32_t marking_previous(const Marking&);
extern "C" int32_t marking_transition(const Marking&);

ffi::CVec<int32_t> Marking::values() const {
    CVec<int32_t> values{};
    ::marking_values(*this, values);
    return values;
}

int32_t Marking::prev() const {
    return ::marking_previous(*this);
}

int32_t Marking::transition() const {
    return ::marking_transition(*this);
}

CVec<Marking *> Reachability::marking() const {
    CVec<Marking*> marking{};
    ::reachability_marking(*this, marking);
    return marking;
}

Reachability::~Reachability() {
    ::reachability_drop(this);
}
