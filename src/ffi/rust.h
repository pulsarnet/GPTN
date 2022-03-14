#ifndef FFI_RUST_RUST_H
#define FFI_RUST_RUST_H

#include <array>
#include <qvariant.h>

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

    struct VertexIndex {
        VertexType type;
        usize id;
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


        Vertex* getVertex(VertexIndex index) const;

        QVariant toVariant() const;
        void fromVariant(const QVariant& data);

        std::pair<CNamedMatrix*, CNamedMatrix*> as_matrix() const;

        void drop();
    };

    struct Vertex {
        VertexIndex index() const;
        usize markers() const;
        void add_marker();
        void remove_marker();
        char* get_name(bool show_parent = true) const;
        void set_name(char* name);
        VertexType type() const;
        void set_parent(VertexIndex);
        usize parent() const;

        QVariant toVariant() const;
    };

    struct Connection {
        VertexIndex from() const;
        VertexIndex to() const;

        QVariant toVariant() const;
    };

    struct DecomposeContext {

        static DecomposeContext* init(PetriNet*);
        static DecomposeContext* fromNets(const QVector<PetriNet*>&);
        usize positions();
        usize transitions();
        CMatrix* primitive_matrix();
        PetriNet* primitive_net();
        usize position_index(usize);
        usize transition_index(usize);
        PetriNet* linear_base_fragments();

        usize programs() const;
        usize program_size(usize index) const;
        void add_program();
        void remove_program(usize index);
        usize program_value(usize program, usize index) const;
        void set_program_value(usize program, usize index, usize value);
        char* program_header_name(usize index, bool label) const;
        CMatrix* c_matrix() const;
        PetriNet* eval_program(usize index);
        PetriNet* program_net_after(usize index) const;
        PetriNet* init_program_after(usize index);

        CVec<PetriNet*> parts() const;
    };

    struct CMatrix {
        i32 index(usize, usize) const;
        void set_value(usize, usize, i32);
        usize rows() const;
        usize columns() const;

        QVariant toVariant() const;
        void fromVariant(const QVariant& data);
    };

    struct CNamedMatrix {
        i32 index(usize, usize) const;
        QString horizontalHeader(usize) const;
        QString verticalHeader(usize) const;
        usize rows() const;
        usize columns() const;
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
