#ifndef FFI_RUST_RUST_H
#define FFI_RUST_RUST_H

#include <array>

namespace ffi {
    extern "C" struct PetriNet;
    extern "C" struct Position;
    extern "C" struct Transition;
    extern "C" struct SynthesisContext;
    extern "C" struct CMatrix;

    typedef uint64_t usize;
    typedef int i32;

    template<typename T>
    class CVec final {

    public:

        [[nodiscard]] usize size() const noexcept;
        const T* data() const noexcept;
        T* data() noexcept;

        const T &index(std::size_t n) const noexcept;
        const T &operator[](std::size_t n) const noexcept;
        T &operator[](std::size_t n) noexcept;

        const std::size_t size_of() const noexcept;

    private:

        std::array<std::uintptr_t, 3> repr;

    };


    struct PetriNet {
        static PetriNet* create();
        CVec<Position> positions();
        CVec<Transition> transitions();
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
        PetriNet* linear_base_fragments();

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
