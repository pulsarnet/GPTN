#ifndef DECOMPOSE_H
#define DECOMPOSE_H

#include <ptn/types.h>
#include <ptn/opaque.h>

namespace ptn::net {
    struct PetriNet;
}

namespace ptn::modules::decompose {
    struct DecomposeContext : public Opaque {
        static DecomposeContext* init(const net::PetriNet*);
        [[nodiscard]] usize positions() const;
        [[nodiscard]] usize transitions() const;
        [[nodiscard]] const net::PetriNet* primitive() const;
        [[nodiscard]] const net::PetriNet* lbf() const;
        [[nodiscard]] usize position_at() const;
        [[nodiscard]] usize transition_at() const;
        void drop();
    };
}

#endif //DECOMPOSE_H
