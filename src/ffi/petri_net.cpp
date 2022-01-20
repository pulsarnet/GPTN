//
// Created by nmuravev on 1/20/2022.
//

#include "petri_net.h"
#include "ffi_parent.h"
#include "rust.h"

InnerPetriNet::InnerPetriNet(FFIBoxedSlice *boxedSlice) {
    QList<QString> r_elements;
    QList<std::pair<QString, QString>> r_connections;
    QList<unsigned long> r_markers;

    auto cursor = boxedSlice->elements;
    for (int i = 0; i < boxedSlice->len_elements; i++) {
        r_elements.push_back(QString(*cursor));
        cursor++;
    }

    auto connections_cursor = boxedSlice->connections;
    for (int i = 0; i < boxedSlice->len_connections; i++) {
        auto s1 = QString((*connections_cursor)->from);
        auto s2 = QString((*connections_cursor)->to);

        r_connections.push_back({s1, s2});
        connections_cursor++;
    }

    auto markers_cursor = boxedSlice->markers;
    for (int i = 0; i < boxedSlice->len_elements; i++) {
        r_markers.push_back(*markers_cursor);
        markers_cursor++;
    }

    this->elements = std::move(r_elements);
    this->connections = std::move(r_connections);
    this->markers = std::move(r_markers);
}

InnerPetriNet::InnerPetriNet(FFINamedMatrix *input, FFINamedMatrix *output) {
    auto in = NamedMatrix(input);
    auto out = NamedMatrix(output);

    this->elements.append(in.rows);
    this->elements.append(in.cols);

    for (int r = 0; r < in.rows.count(); r++) {
        for (int c = 0; c < in.cols.count(); c++) {
            if (in(r, c) != 0) {
                this->connections.push_back({in.rows[r], in.cols[c]});
            }
            else if (out(r, c) != 0) {
                this->connections.push_back({in.cols[c], in.rows[r]});
            }
        }
    }
}

QList<InnerPetriNet*> InnerPetriNet::from_ffi_linear_base_fragments(FFILogicalBaseFragments *lbf) {
    auto input_cursor = lbf->inputs;
    auto output_cursor = lbf->outputs;

    QList<InnerPetriNet*> result;

    for (int i = 0; i < lbf->len; i++) {

        auto net = new InnerPetriNet(input_cursor, output_cursor);
        result.push_back(net);
        input_cursor++;
        output_cursor++;
    }

    return result;
}

InnerCommonResult::InnerCommonResult(CommonResult *ffi) {
    this->net = InnerPetriNet(ffi->petri_net);
    this->c_matrix = Matrix(ffi->c_matrix);
    this->lbf_matrix = NamedMatrix(ffi->lbf_matrix);
    this->fragments = InnerPetriNet::from_ffi_linear_base_fragments(ffi->logical_base_fragments);
    this->parents = FFIParent::from_ffi_parent_vec(ffi->parents);
}