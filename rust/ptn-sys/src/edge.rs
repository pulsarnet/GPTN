use ptn::net::Connection;
use ptn::net::vertex::VertexIndex;

#[export_name = "ptn$net$edge$from"]
extern "C" fn edge_from(edge: &Connection) -> VertexIndex {
    edge.first()
}

#[export_name = "ptn$net$edge$to"]
extern "C" fn edge_to(edge: &Connection) -> VertexIndex {
    edge.second()
}

#[export_name = "ptn$net$edge$weight"]
extern "C" fn edge_weight(edge: &Connection) -> usize {
    edge.weight()
}

#[export_name = "ptn$net$edge$set_weight"]
pub extern "C" fn edge_set_weight(edge: &mut Connection, weight: usize) {
    edge.set_weight(weight);
}
