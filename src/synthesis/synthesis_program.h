//
// Created by Николай Муравьев on 09.01.2022.
//

#ifndef FFI_RUST_SYNTHESIS_PROGRAM_H
#define FFI_RUST_SYNTHESIS_PROGRAM_H

#include <QList>

struct PetriNet;
struct FFIParent;
struct InnerPetriNet;
struct InnerCommonResult;
struct CommonResult;

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

    QList<InnerPetriNet*> linear_base_fragments();

    QList<FFIParent*> parents_vec();

    InnerCommonResult* eval(unsigned long program);

    static SynthesisProgram* synthesis_start(PetriNet*);

    static CommonResult* synthesis_end(SynthesisProgram*);
};

#endif //FFI_RUST_SYNTHESIS_PROGRAM_H
