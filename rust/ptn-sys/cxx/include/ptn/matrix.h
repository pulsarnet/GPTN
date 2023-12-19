#ifndef MATRIX_H
#define MATRIX_H

#include <ptn/opaque.h>

namespace ptn::matrix {

    template<typename T>
    class RustMatrix<T> : public Opaque {

    };

}


#endif //MATRIX_H
