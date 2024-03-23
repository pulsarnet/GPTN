#ifndef CONTEXT_H
#define CONTEXT_H

#include <ptn/opaque.h>

namespace ptn::net {
    struct PetriNet;
}

namespace ptn::modules::decompose {
    struct DecomposeContext;
}

namespace ptn::net::context {

    struct PetriNetContext : public Opaque {
        static PetriNetContext* create();
        [[nodiscard]] PetriNet* net() const;
        [[nodiscard]] ptn::modules::decompose::DecomposeContext* decompose_ctx() const;
        void decompose();
        void set_decompose_ctx(modules::decompose::DecomposeContext* ctx);

        void drop();

        ~PetriNetContext() = delete;
    };

}

#endif //CONTEXT_H