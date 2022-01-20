//
// Created by nmuravev on 12/20/2021.
//

#ifndef FFI_RUST_RUST_H
#define FFI_RUST_RUST_H

#include <iostream>
#include "../petri_net.h"
#include "../matrix.h"
#include "../named_matrix.h"
#include "../synthesis/synthesis_program.h"
#include "net.h"

extern "C" struct FFIConnection {
    char* from;
    char* to;
};

extern "C" struct FFIBoxedSlice {
    char** elements;
    unsigned long len_elements;
    FFIConnection** connections;
    unsigned long len_connections;
    unsigned long* markers;

    [[nodiscard]] InnerPetriNet into() const {
        QList<QString> r_elements;
        QList<std::pair<QString, QString>> r_connections;
        QList<unsigned long> r_markers;

        auto cursor = this->elements;
        for (int i = 0; i < len_elements; i++) {
            r_elements.push_back(QString(*cursor));
            cursor++;
        }

        auto connections_cursor = this->connections;
        for (int i = 0; i < len_connections; i++) {
            auto s1 = QString((*connections_cursor)->from);
            auto s2 = QString((*connections_cursor)->to);

            r_connections.push_back({s1, s2});
            connections_cursor++;
        }

        auto markers_cursor = this->markers;
        for (int i = 0; i < len_elements; i++) {
            r_markers.push_back(*markers_cursor);
            markers_cursor++;
        }

        return InnerPetriNet{ std::move(r_elements), std::move(r_connections), std::move(r_markers)};
    }
};

extern "C" struct FFIMatrix {
    unsigned long rows_len;
    unsigned long cols_len;
    long* matrix;

    [[nodiscard]] Matrix into() const {
        QList<long> elements;
        elements.resize(rows_len * cols_len, 0);

        for (int i = 0; i < rows_len; i++) {
            for (int j = 0; j < cols_len; j++) {
                elements[cols_len * i + j] = *(matrix + cols_len * i + j);
            }
        }

        return Matrix{rows_len, cols_len, elements};
    }
};

extern "C" struct FFINamedMatrix {
    char** rows;
    unsigned long rows_len;

    char** cols;
    unsigned long cols_len;
    long* matrix;

    [[nodiscard]] NamedMatrix into() const {
        QList<long> elements;
        elements.resize(rows_len * cols_len, 0);

        for (int i = 0; i < rows_len; i++) {
            for (int j = 0; j < cols_len; j++) {
                elements[cols_len * i + j] = *(matrix + cols_len * i + j);
            }
        }

        QList<QString> rows;
        QList<QString> cols;

        auto cursor = this->rows;
        for (int i = 0; i < rows_len; i++) {
            rows.push_back(QString(*cursor));
            cursor++;
        }

        cursor = this->cols;
        for (int i = 0; i < cols_len; i++) {
            cols.push_back(QString(*cursor));
            cursor++;
        }

        return NamedMatrix{rows, cols,elements};
    }
};

extern "C" struct FFILogicalBaseFragments {
    FFINamedMatrix* inputs;
    FFINamedMatrix* outputs;

    unsigned long long len;

    QList<InnerPetriNet> into() {

        auto input_cursor = inputs;
        auto output_cursor = outputs;

        QList<InnerPetriNet> result;

        for (int i = 0; i < len; i++) {

            auto net = InnerPetriNet();

            auto in = input_cursor->into();
            auto out = output_cursor->into();

            net.elements.append(in.rows);
            net.elements.append(in.cols);

            for (int r = 0; r < in.rows.count(); r++) {
                for (int c = 0; c < in.cols.count(); c++) {
                    if (in(r, c) != 0) {
                        net.connections.push_back({in.rows[r], in.cols[c]});
                    }
                    else if (out(r, c) != 0) {
                        net.connections.push_back({in.cols[c], in.rows[r]});
                    }
                }
            }

            result.push_back(net);

            input_cursor++;
            output_cursor++;
        }

        return result;
    }
};

extern "C" struct CommonResult {
    FFIBoxedSlice* petri_net;
    FFIMatrix* c_matrix;
    FFINamedMatrix* lbf_matrix;
    FFILogicalBaseFragments* logical_base_fragments;
    FFIParentVec* parents;
};

extern "C" SynthesisProgram* synthesis_start(PetriNet*);
extern "C" CommonResult* synthesis_end(SynthesisProgram*);

SynthesisProgram* split_net(PetriNet* net);


#endif //FFI_RUST_RUST_H
