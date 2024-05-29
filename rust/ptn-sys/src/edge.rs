use ptn::net::DirectedEdge;
use ptn::net::InhibitorEdge;
use ptn::net::vertex::VertexIndex;

#[export_name = "ptn$net$edge$directed$from"]
extern "C" fn edge_from(edge: &DirectedEdge) -> VertexIndex {
    edge.begin()
}

#[export_name = "ptn$net$edge$directed$to"]
extern "C" fn edge_to(edge: &DirectedEdge) -> VertexIndex {
    edge.end()
}

#[export_name = "ptn$net$edge$directed$weight"]
extern "C" fn edge_weight(edge: &DirectedEdge) -> u32 {
    edge.weight()
}

#[export_name = "ptn$net$edge$directed$set_weight"]
pub extern "C" fn edge_set_weight(edge: &mut DirectedEdge, weight: u32) {
    edge.set_weight(weight);
}

#[export_name = "ptn$net$edge$inhibitor$place"]
extern "C" fn inhibitor_place(edge: &InhibitorEdge) -> VertexIndex {
    edge.place()
}

#[export_name = "ptn$net$edge$inhibitor$transition"]
extern "C" fn inhibitor_transition(edge: &InhibitorEdge) -> VertexIndex {
    edge.transition()
}
