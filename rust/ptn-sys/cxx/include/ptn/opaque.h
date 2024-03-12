#ifndef PTN_OPAQUE_H
#define PTN_OPAQUE_H

namespace ptn {
    struct Opaque {
        Opaque() = delete;
        Opaque(const Opaque &) = delete;
        ~Opaque() = delete;
    };
}

#endif