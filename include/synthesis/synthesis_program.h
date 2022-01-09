//
// Created by Николай Муравьев on 09.01.2022.
//

#ifndef FFI_RUST_SYNTHESIS_PROGRAM_H
#define FFI_RUST_SYNTHESIS_PROGRAM_H

#include <QList>

extern "C" struct SynthesisProgram;

extern "C" unsigned long positions(SynthesisProgram*);
extern "C" unsigned long transitions(SynthesisProgram*);

extern "C" char* position(SynthesisProgram*, unsigned long);
extern "C" char* transition(SynthesisProgram*, unsigned long);

extern "C" unsigned long programs(SynthesisProgram*);
extern "C" void add_program(SynthesisProgram*);

extern "C" void set_program_value(SynthesisProgram*, unsigned long, unsigned long, unsigned long);
extern "C" unsigned long get_program_value(SynthesisProgram*, unsigned long, unsigned long);

struct SynthesisProgram {
    unsigned long positions() {
        return ::positions(this);
    }

    unsigned long transitions() {
        return ::transitions(this);
    }

    unsigned long programs() {
        return ::programs(this);
    }

    void add_program() {
        ::add_program(this);
    }

    char* position(unsigned long index) {
        return ::position(this, index);
    }

    char* transition(unsigned long index) {
        return ::transition(this, index);
    }

    void set_program_value(unsigned long program, unsigned long element, unsigned long value) {
        ::set_program_value(this, program, element, value);
    }

    unsigned long get_program_value(unsigned long program, unsigned long element) {
        return ::get_program_value(this, program, element);
    }
};

#endif //FFI_RUST_SYNTHESIS_PROGRAM_H
