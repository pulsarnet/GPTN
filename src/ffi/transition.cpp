//
// Created by Николай Муравьев on 10.01.2022.
//

#include "../../include/ffi/methods.h"

unsigned long FFITransition::index() {
    return ::transition_index(this);
}
