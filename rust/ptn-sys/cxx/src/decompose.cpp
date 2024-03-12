#include <ptn/decompose.h>
#include <ptn/net.h>

namespace ptn::modules::decompose {
    extern "C" {
        DecomposeContext* ptn$modules$decompose$init(const net::PetriNet* net);
        usize ptn$modules$decompose$positions(const DecomposeContext* ctx);
        usize ptn$modules$decompose$transitions(const DecomposeContext* ctx);
        net::PetriNet* ptn$modules$decompose$primitive(const DecomposeContext* ctx);
        net::PetriNet* ptn$modules$decompose$lbf(const DecomposeContext* ctx);
        usize ptn$modules$decompose$position_at(const DecomposeContext* ctx);
        usize ptn$modules$decompose$transition_at(const DecomposeContext* ctx);
        usize ptn$modules$decompose$programs(const DecomposeContext* ctx);
        net::PetriNet* ptn$modules$decompose$eval_program(const DecomposeContext* ctx, usize idx);
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

    net::PetriNet* DecomposeContext::primitive() const {
        return ptn$modules$decompose$primitive(this);
    }

    net::PetriNet* DecomposeContext::lbf() const {
        return ptn$modules$decompose$lbf(this);
    }

    usize DecomposeContext::position_at() const {
        return ptn$modules$decompose$position_at(this);
    }

    usize DecomposeContext::transition_at() const {
        return ptn$modules$decompose$transition_at(this);
    }

    usize DecomposeContext::programs() const noexcept {
        return ptn$modules$decompose$programs(this);
    }

    net::PetriNet* DecomposeContext::eval_program(size_t idx) const {
        return ptn$modules$decompose$eval_program(this, idx);
    }


    void DecomposeContext::drop() {
        ptn$modules$decompose$drop(this);
    }
}