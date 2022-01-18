//
// Created by Николай Муравьев on 09.01.2022.
//
#include "rust.h"

InnerCommonResult split_finish(SynthesisProgram *program) {
    auto common = synthesis_end(program);
    return InnerCommonResult {
            common->petri_net->into(),
            common->c_matrix->into(),
            common->lbf_matrix->into(),
            common->logical_base_fragments->into(),
            common->parents->into()
    };
}

SynthesisProgram *split_net(PetriNet *net) {
    return synthesis_start(net);
}
