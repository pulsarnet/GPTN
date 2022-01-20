//
// Created by Николай Муравьев on 09.01.2022.
//

#ifndef FFI_RUST_SYNTHESIS_PROGRAM_H
#define FFI_RUST_SYNTHESIS_PROGRAM_H

#include <QList>

struct InnerCommonResult;

extern "C" struct SynthesisProgram;

struct SynthesisProgram {
    unsigned long positions();

    unsigned long transitions();

    unsigned long programs();

    void add_program();

    char* position(unsigned long index);

    char* transition(unsigned long index);

    void set_program_value(unsigned long program, unsigned long element, unsigned long value);

    unsigned long get_program_value(unsigned long program, unsigned long element);

    InnerCommonResult* eval(unsigned long program);
};

#endif //FFI_RUST_SYNTHESIS_PROGRAM_H
