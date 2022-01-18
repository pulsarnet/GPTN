//
// Created by Николай Муравьев on 10.01.2022.
//


#include "methods.h"

unsigned long FFIPosition::index() {
    return ::position_index(this);
}

unsigned long FFIPosition::markers() {
    return ::markers(this);
}

void FFIPosition::add_marker() {
    ::add_marker(this);
}

void FFIPosition::set_markers(unsigned long markers) {
    ::set_markers(this, markers);
}

void FFIPosition::remove_marker() {
    ::remove_marker(this);
}
