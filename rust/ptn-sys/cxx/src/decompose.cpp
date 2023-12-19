#include <ptn/decompose.h>
#include <ptn/net.h>

namespace ptn::modules::decompose {
    extern "C" {
        DecomposeContext* ptn$modules$decompose$init(const net::PetriNet* net);
        usize ptn$modules$decompose$positions(const DecomposeContext* ctx);
        usize ptn$modules$decompose$transitions(const DecomposeContext* ctx);
        const net::PetriNet* ptn$modules$decompose$primitive(const DecomposeContext* ctx);
        const net::PetriNet* ptn$modules$decompose$lbf(const DecomposeContext* ctx);
        usize ptn$modules$decompose$position_at(const DecomposeContext* ctx);
        usize ptn$modules$decompose$transition_at(const DecomposeContext* ctx);
        void ptn$modules$decompose$drop(DecomposeContext* ctx);
    }

    DecomposeContext* DecomposeContext::init(const net::PetriNet* net) {
        return ptn$modules$decompose$init(net);
    }

    usize DecomposeContext::positions() const {
        return ptn$modules$decompose$positions(this);
    }

    usize DecomposeContext::transitions() const {
        return ptn$modules$decompose$transitions(this);
    }

    const net::PetriNet* DecomposeContext::primitive() const {
        return ptn$modules$decompose$primitive(this);
    }

    const net::PetriNet* DecomposeContext::lbf() const {
        return ptn$modules$decompose$lbf(this);
    }

    usize DecomposeContext::position_at() const {
        return ptn$modules$decompose$position_at(this);
    }

    usize DecomposeContext::transition_at() const {
        return ptn$modules$decompose$transition_at(this);
    }

    void DecomposeContext::drop() {
        ptn$modules$decompose$drop(this);
    }
}