//
// Created by Николай Муравьев on 09.01.2022.
//
#include "../../include/ffi/rust.h"

InnerCommonResult split_finish(SynthesisProgram *program) {
    auto common = synthesis_end(program);
    return InnerCommonResult {
            common->petri_net->into(),
            common->c_matrix->into(),
            common->lbf_matrix->into()
    };
}

SynthesisProgram *split_net(PetriNet *net) {
    return synthesis_start(net);
}
