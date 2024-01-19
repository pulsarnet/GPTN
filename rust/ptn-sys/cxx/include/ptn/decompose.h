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
        [[nodiscard]] net::PetriNet* primitive() const;
        [[nodiscard]] net::PetriNet* lbf() const;
        [[nodiscard]] usize position_at() const;
        [[nodiscard]] usize transition_at() const;
        void drop();

        // synthesis
        [[nodiscard]] usize programs() const noexcept;
        [[nodiscard]] net::PetriNet* eval_program(size_t idx) const;

        ~DecomposeContext() = delete;
    };
}

#endif //DECOMPOSE_H
