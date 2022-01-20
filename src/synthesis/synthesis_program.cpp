//
// Created by nmuravev on 1/20/2022.
//

#include "synthesis_program.h"
#include "../ffi/rust.h"
#include "../ffi/ffi_parent.h"
#include "../ffi/petri_net.h"

extern "C" unsigned long positions(SynthesisProgram*);
extern "C" unsigned long transitions(SynthesisProgram*);

extern "C" char* position(SynthesisProgram*, unsigned long);
extern "C" char* transition(SynthesisProgram*, unsigned long);

extern "C" unsigned long programs(SynthesisProgram*);
extern "C" void add_program(SynthesisProgram*);

extern "C" void set_program_value(SynthesisProgram*, unsigned long, unsigned long, unsigned long);
extern "C" unsigned long get_program_value(SynthesisProgram*, unsigned long, unsigned long);

extern "C" CommonResult* eval_program(SynthesisProgram*, unsigned long);
extern "C" FFILogicalBaseFragments* linear_base_fragments(SynthesisProgram*);
extern "C" FFIParentVec* parents_vec(SynthesisProgram*);

extern "C" SynthesisProgram* synthesis_start(PetriNet*);
extern "C" CommonResult* synthesis_end(SynthesisProgram*);

SynthesisProgram* split_net(PetriNet* net);

InnerCommonResult* SynthesisProgram::eval(unsigned long program) {
    auto eval_res = ::eval_program(this, program);
    return new InnerCommonResult(eval_res);
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

QList<InnerPetriNet*> SynthesisProgram::linear_base_fragments() {
    return std::move(InnerPetriNet::from_ffi_linear_base_fragments(::linear_base_fragments(this)));
}

QList<FFIParent*> SynthesisProgram::parents_vec() {
    return std::move(FFIParent::from_ffi_parent_vec(::parents_vec(this)));
}

SynthesisProgram *SynthesisProgram::synthesis_start(PetriNet *net) {
    return ::synthesis_start(net);
}

CommonResult *SynthesisProgram::synthesis_end(SynthesisProgram *self) {
    return ::synthesis_end(self);
}
