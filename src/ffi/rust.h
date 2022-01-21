#ifndef FFI_RUST_RUST_H
#define FFI_RUST_RUST_H

namespace ffi {
    extern "C" struct PetriNet;
    extern "C" struct Position;
    extern "C" struct Transition;
    extern "C" struct SynthesisContext;
    extern "C" struct CMatrix;

    typedef unsigned __int64 usize;
    typedef int i32;

    extern "C" {
        // PetriNet
        PetriNet* create_net();
        Position* add_position(PetriNet&);
        Position* add_position_with(PetriNet&, usize);
        Position* get_position(PetriNet&, usize);
        void remove_position(PetriNet&, Position&);
        Transition* add_transition(PetriNet&);
        Transition* add_transition_with(PetriNet&, usize);
        Transition* get_transition(PetriNet&, usize);
        void remove_transition(PetriNet&, Transition&);
        void connect_p(PetriNet&, Position&, Transition&);
        void connect_t(PetriNet&, Transition&, Position&);
        void remove_connection_p(PetriNet&, Position&, Transition&);
        void remove_connection_t(PetriNet&, Transition&, Position&);

        // Position
        usize position_index(Position&);
        usize position_markers(Position&);
        void position_add_marker(Position&);
        void position_remove_marker(Position&);

        // Transition
        usize transition_index(Transition&);

        // SynthesisContext
        SynthesisContext* synthesis_init(PetriNet&);
        usize synthesis_positions(SynthesisContext&);
        usize synthesis_transitions(SynthesisContext&);
        usize synthesis_programs(SynthesisContext&);
        CMatrix* synthesis_c_matrix(SynthesisContext&);
        CMatrix* synthesis_primitive_matrix(SynthesisContext&);
        usize synthesis_position_index(SynthesisContext&, usize);
        usize synthesis_transition_index(SynthesisContext&, usize);

        // CMatrix
        i32 matrix_index(CMatrix&, usize, usize);
        usize matrix_rows(CMatrix&);
        usize matrix_columns(CMatrix&);
    };

    struct PetriNet {
        static PetriNet* create();
        Position* add_position();
        Position* add_position_with(usize);
        Position* get_position(usize);
        void remove_position(Position*);
        Transition* add_transition();
        Transition* add_transition_with(usize);
        Transition* get_transition(usize);
        void remove_transition(Transition*);
        void connect_p(Position*, Transition*);
        void connect_t(Transition*, Position*);
        void remove_connection_p(Position*, Transition*);
        void remove_connection_t(Transition*, Position*);
    };

    struct Position {
        usize index();
        usize markers();
        void add_marker();
        void remove_marker();
    };

    struct Transition {
        usize index();
    };

    struct SynthesisContext {

        static SynthesisContext* init(PetriNet*);
        usize positions();
        usize transitions();
        usize programs();
        CMatrix* c_matrix();
        CMatrix* primitive_matrix();
        usize position_index(usize);
        usize transition_index(usize);

    };

    struct CMatrix {
        i32 index(usize, usize);
        usize rows();
        usize columns();
    };

}


#endif //FFI_RUST_RUST_H
