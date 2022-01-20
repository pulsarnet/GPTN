//
// Created by nmuravev on 1/20/2022.
//

#include "synthesis_program.h"
#include "../ffi/rust.h"

extern "C" unsigned long positions(SynthesisProgram*);
extern "C" unsigned long transitions(SynthesisProgram*);

extern "C" char* position(SynthesisProgram*, unsigned long);
extern "C" char* transition(SynthesisProgram*, unsigned long);

extern "C" unsigned long programs(SynthesisProgram*);
extern "C" void add_program(SynthesisProgram*);

extern "C" void set_program_value(SynthesisProgram*, unsigned long, unsigned long, unsigned long);
extern "C" unsigned long get_program_value(SynthesisProgram*, unsigned long, unsigned long);

extern "C" CommonResult* eval_program(SynthesisProgram*, unsigned long);

InnerCommonResult* SynthesisProgram::eval(unsigned long program) {
    auto eval_res = ::eval_program(this, program);
    return new InnerCommonResult {
            eval_res->petri_net->into(),
            eval_res->c_matrix->into(),
            eval_res->lbf_matrix->into(),
            eval_res->logical_base_fragments->into(),
            eval_res->parents->into()
    };
}

unsigned long SynthesisProgram::positions() {
    return ::positions(this);
}

unsigned long SynthesisProgram::transitions() {
    return ::transitions(this);
}

unsigned long SynthesisProgram::get_program_value(unsigned long program, unsigned long element) {
    return ::get_program_value(this, program, element);
}

void SynthesisProgram::set_program_value(unsigned long program, unsigned long element, unsigned long value) {
    ::set_program_value(this, program, element, value);
}

char *SynthesisProgram::transition(unsigned long index) {
    return ::transition(this, index);
}

void SynthesisProgram::add_program() {
    ::add_program(this);
}

char *SynthesisProgram::position(unsigned long index) {
    return ::position(this, index);
}

unsigned long SynthesisProgram::programs() {
    return ::programs(this);
}
