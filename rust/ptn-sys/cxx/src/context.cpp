#include <ptn/context.h>

namespace ptn::net::context {

    extern "C" {
        PetriNetContext* ptn$net$context$create();
        PetriNet* ptn$net$context$net(const PetriNetContext*);
        modules::decompose::DecomposeContext* ptn$net$context$decompose_ctx(const PetriNetContext*);
        void ptn$net$context$decompose(PetriNetContext*);
        void ptn$net$context$set_decompose_ctx(PetriNetContext*, modules::decompose::DecomposeContext*);
        void ptn$net$context$drop(PetriNetContext*);
    }

    PetriNetContext* PetriNetContext::create() {
        return ptn$net$context$create();
    }

    PetriNet* PetriNetContext::net() const {
        return ptn$net$context$net(this);
    }

    modules::decompose::DecomposeContext* PetriNetContext::decompose_ctx() const {
        return ptn$net$context$decompose_ctx(this);
    }

    void PetriNetContext::decompose() {
        ptn$net$context$decompose(this);
    }

    void PetriNetContext::set_decompose_ctx(modules::decompose::DecomposeContext* ctx) {
        ptn$net$context$set_decompose_ctx(this, ctx);
    }

    void PetriNetContext::drop() {
        ptn$net$context$drop(this);
    }

}