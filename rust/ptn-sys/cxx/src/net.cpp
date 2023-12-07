#include "net.h"

namespace net = ptn::net;
namespace vertex = net::vertex;

extern "C" {
    net::PetriNet* ptn$net$make();
    void ptn$net$vertex(const net::PetriNet&, vertex::VertexIndex);
}