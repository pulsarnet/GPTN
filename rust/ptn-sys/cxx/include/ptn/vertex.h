#ifndef PTN_NET_VERTEX
#define PTN_NET_VERTEX

#include <ptn/types.h>

namespace ptn::net::vertex {

    enum VertexType {
        Position,
        Transition
    };

    struct VertexIndex {
        VertexType t;
        usize id;
    };


    struct Vertex {
        [[nodiscard]] VertexIndex index() const;
        [[nodiscard]] usize markers() const;
        void add_marker();
        void remove_marker();
        void set_markers(usize);
        [[nodiscard]] char* label(bool) const;
        void set_label(char*);
        [[nodiscard]] VertexType type() const;
        [[nodiscard]] VertexIndex parent() const;
        void set_parent(usize);
    };

}

#endif