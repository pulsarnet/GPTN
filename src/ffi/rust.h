#ifndef FFI_RUST_RUST_H
#define FFI_RUST_RUST_H

#include <array>
#include <qvariant.h>
#include "vec.h"
#include "types.h"

namespace rust {
    struct Reachability;
}

namespace ffi {

    struct CNamedMatrix;

    extern "C" void init();

    extern "C" enum VertexType {
        Position = 0x0,
        Transition = 0x1
    };

    extern "C" struct PetriNet;
    extern "C" struct Vertex;
    extern "C" struct CMatrix;
    extern "C" struct Connection;
    extern "C" struct PetriNetContext;
    extern "C" struct DecomposeContext;

    struct VertexIndex {
        VertexType type;
        usize id;

        bool operator==(const VertexIndex& other) const {
            return type == other.type && id == other.id;
        }

        bool operator!=(const VertexIndex& other) const {
            return !(*this == other);
        }
    };

    struct PetriNetContext {
        static PetriNetContext* create();
        static void free(PetriNetContext* context);

        void decompose();

        PetriNet* net() const;
        DecomposeContext* decompose_ctx() const;
        void set_decompose_ctx(DecomposeContext* ctx);
    };

    struct PetriNet {
        static PetriNet* create();

        CVec<Vertex*> positions() const;
        CVec<Vertex*> transitions() const;
        CVec<Connection*> connections() const;
        Vertex* add_position();
        Vertex* add_position_with(usize);
        Vertex* add_position_with_parent(usize, usize);
        Vertex* get_position(usize);
        void remove_position(Vertex*);
        Vertex* add_transition();
        Vertex* add_transition_with(usize);
        Vertex* add_transition_with_parent(usize, usize);
        Vertex* get_transition(usize);
        void remove_transition(Vertex*);
        void connect(Vertex*, Vertex*);
        void remove_connection(Vertex*, Vertex*);
        rust::Reachability* reachability() const;
        Connection* get_connection(Vertex*, Vertex*);

        Vertex* getVertex(VertexIndex index) const;

        std::pair<CNamedMatrix*, CNamedMatrix*> as_matrix() const;

        void clear();
        void drop();

        usize input_positions();
        usize output_positions();
        usize connection_weight(Vertex*, Vertex*);
        
        // invariant
        void p_invariant() const;
        void t_invariant() const;
    };

    struct Vertex {
        VertexIndex index() const;
        usize markers() const;
        void add_marker();
        void remove_marker();
        void set_markers(usize markers);
        char* get_name(bool show_parent = true) const;
        void set_name(char* name);
        VertexType type() const;
        void set_parent(VertexIndex);
        usize parent() const;
    };

    struct Connection {
        VertexIndex from() const;
        VertexIndex to() const;
        
        usize weight() const;
        void setWeight(usize weight);
    };

    struct DecomposeContext {

        static DecomposeContext* init(PetriNet*);
        static DecomposeContext* fromNets(const QVector<PetriNet*>&);
        usize positions();
        usize transitions();
        PetriNet* primitive_net();
        usize position_index(usize);
        usize transition_index(usize);
        PetriNet* linear_base_fragments();

        usize programs() const;
        usize program_size(usize index) const;
        usize program_value(usize program, usize index) const;
        void set_program_value(usize program, usize index, usize value);
        char* program_header_name(usize index, bool label) const;
        PetriNet* eval_program(usize index);
        QString program_equations(usize index) const;
        usize transition_united(usize index);
        usize position_united(usize index);
        void calculate_all() const;

        CVec<PetriNet*> parts() const;
    };

    struct CMatrix {
        i32 index(usize, usize) const;
        void set_value(usize, usize, i32);
        usize rows() const;
        usize columns() const;
    };

    struct CNamedMatrix {
        i32 index(usize, usize) const;
        QString horizontalHeader(usize) const;
        QString verticalHeader(usize) const;
        usize rows() const;
        usize columns() const;
    };
}

template<>
struct std::hash<ffi::VertexIndex> {
    std::size_t operator()(const ffi::VertexIndex& index) const {
        return std::hash<int>()(index.type) ^ std::hash<ffi::usize>()(index.id);
    }
};

#endif //FFI_RUST_RUST_H
