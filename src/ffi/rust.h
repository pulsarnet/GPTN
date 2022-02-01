#ifndef FFI_RUST_RUST_H
#define FFI_RUST_RUST_H

#include <array>

namespace ffi {

    extern "C" enum VertexType {
        Position = 0x0,
        Transition = 0x1
    };

    extern "C" struct PetriNet;
    extern "C" struct Vertex;
    extern "C" struct SynthesisContext;
    extern "C" struct CMatrix;
    extern "C" struct Connection;

    typedef uint64_t usize;
    typedef int i32;

    template<typename T>
    class CVec final {

        typedef T value_type;
        typedef T* pointer_type;

    public:

        [[nodiscard]] usize size() const noexcept;
        value_type const* data() const noexcept;
        pointer_type data() noexcept;

        const T &index(std::size_t n) const noexcept;
        const T &operator[](std::size_t n) const noexcept;
        T &operator[](std::size_t n) noexcept;

        [[nodiscard]] const std::size_t size_of() const noexcept;

    private:

        std::array<std::uintptr_t, 3> repr;

    };


    struct PetriNet {
        static PetriNet* create();
        CVec<Vertex*> positions();
        CVec<Vertex*> transitions();
        CVec<Connection*> connections();
        Vertex* add_position();
        Vertex* add_position_with(usize);
        Vertex* get_position(usize);
        void remove_position(Vertex*);
        Vertex* add_transition();
        Vertex* add_transition_with(usize);
        Vertex* get_transition(usize);
        void remove_transition(Vertex*);
        void connect(Vertex*, Vertex*);
        void remove_connection(Vertex*, Vertex*);
    };

    struct Vertex {
        usize index();
        usize markers();
        void add_marker();
        void remove_marker();
        char* get_name();
        void set_name(char* name);
        VertexType type();
    };

    struct Connection {
        Vertex* from();
        Vertex* to();
    };

    struct SynthesisContext {

        static SynthesisContext* init(PetriNet*);
        usize positions();
        usize transitions();
        usize programs();
        void add_program();
        void remove_program(usize index);
        usize program_value(usize program, usize index);
        void set_program_value(usize program, usize index, usize value);
        char* program_header_name(usize index);
        CMatrix* c_matrix();
        CMatrix* primitive_matrix();
        PetriNet* primitive_net();
        usize position_index(usize);
        usize transition_index(usize);
        PetriNet* linear_base_fragments();
        PetriNet* eval_program(usize index);
    };

    struct CMatrix {
        i32 index(usize, usize);
        usize rows();
        usize columns();
    };

    template<typename T>
    T *CVec<T>::data() noexcept {
        return const_cast<T*>(const_cast<const CVec<T>*>(this)->data());
    }

    template<typename T>
    const T &CVec<T>::index(std::size_t n) const noexcept {
        auto data = reinterpret_cast<const char*>(this->data());
        return *reinterpret_cast<const T*>(data + n * size_of());
    }

    template<typename T>
    T& CVec<T>::operator[](std::size_t n) noexcept  {
        auto data = reinterpret_cast<char*>(this->data());
        return *reinterpret_cast<T*>(data + n * size_of());
    }

    template<typename T>
    const T& CVec<T>::operator[](std::size_t n) const noexcept  {
        auto data = reinterpret_cast<const char*>(this->data());
        return *reinterpret_cast<const T*>(data + n * size_of());
    }
}


#endif //FFI_RUST_RUST_H
