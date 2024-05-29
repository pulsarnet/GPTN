pub use net::edge::direct::DirectedEdge;
pub use net::edge::inhibitor::InhibitorEdge;
use net::vertex::VertexIndex;

mod direct;
mod inhibitor;

#[derive(Debug, Default, Clone)]
pub struct Edges {
    /// Array of directed edges
    directed: Vec<DirectedEdge>,
    
    /// Array of inhibitor edges
    inhibitor: Vec<InhibitorEdge>
}

impl Edges {
    pub fn directed(&self) -> &[DirectedEdge] {
        &self.directed
    }

    pub fn add_directed(&mut self, edge: DirectedEdge) {
        match self.get_directed_mut(edge.begin(), edge.end()) {
            Some(directed) => directed.set_weight(directed.weight() + edge.weight()),
            None => self.directed.push(edge)
        }
    }

    pub fn remove_directed(&mut self, from: VertexIndex, to: VertexIndex) -> Option<DirectedEdge> {
        let idx = self.directed.iter().position(|x| x.begin() == from && x.end() == to)?;
        Some(self.directed.remove(idx))
    }
    
    pub fn has_directed(&self) -> bool {
        !self.directed.is_empty()
    }

    pub fn get_directed(&self, from: VertexIndex, to: VertexIndex) -> Option<&DirectedEdge> {
        assert_ne!(from.type_, to.type_, "'from' and 'to' should have different type");
        self.directed.iter().find(|e| e.begin() == from && e.end() == to)
    }

    pub fn get_directed_mut(&mut self, from: VertexIndex, to: VertexIndex) -> Option<&mut DirectedEdge> {
        assert_ne!(from.type_, to.type_, "'from' and 'to' should have different type");
        self.directed.iter_mut().find(|e| e.begin() == from && e.end() == to)
    }
    
    pub fn inhibitor(&self) -> &[InhibitorEdge] {
        &self.inhibitor
    }

    pub fn add_inhibitor(&mut self, edge: InhibitorEdge) {
        if self.get_inhibitor_mut(edge.place(), edge.transition()).is_none() {
            self.inhibitor.push(edge)
        }
    }

    pub fn remove_inhibitor(&mut self, from: VertexIndex, to: VertexIndex) {
        assert_ne!(from.type_, to.type_, "'from' and 'to' should have different type");
        let idx = self.inhibitor.iter().position(|x| x.place() == from && x.transition() == to);
        if let Some(idx) = idx {
            self.inhibitor.remove(idx);
        }
    }

    pub fn has_inhibitor(&self) -> bool {
        !self.inhibitor.is_empty()
    }

    pub fn get_inhibitor(&self, from: VertexIndex, to: VertexIndex) -> Option<&InhibitorEdge> {
        self.inhibitor.iter().find(|e| e.place() == from && e.transition() == to)
    }

    pub fn get_inhibitor_mut(&mut self, from: VertexIndex, to: VertexIndex) -> Option<&mut InhibitorEdge> {
        self.inhibitor.iter_mut().find(|e| e.place() == from && e.transition() == to)
    }
    
    pub fn remove_relative_to_vertex(&mut self, idx: VertexIndex) {
        self.directed.retain(|e| e.begin() != idx && e.end() != idx);
        self.inhibitor.retain(|e| e.place() != idx && e.transition() != idx);
    }
}

#[cfg(test)]
mod tests {
    use net::{DirectedEdge, Edges, InhibitorEdge};
    use net::vertex::VertexIndex;

    #[test]
    fn add_edge() {
        let mut edges = Edges::default();
        
        let directed_t1_p1 = DirectedEdge::new(VertexIndex::transition(1), VertexIndex::position(1));
        let directed_t1_p2 = DirectedEdge::new(VertexIndex::transition(1), VertexIndex::position(2));
        
        edges.add_directed(directed_t1_p1);
        edges.add_directed(directed_t1_p2);
        assert_eq!(edges.directed[0], directed_t1_p1);
        assert_eq!(edges.directed[1], directed_t1_p2);
    }
    
    #[test]
    fn remove_relative_to_vertex() {
        let mut edges = Edges::default();
        let directed_t1_p1 = DirectedEdge::new(VertexIndex::transition(1), VertexIndex::position(1));
        let directed_t2_p2 = DirectedEdge::new(VertexIndex::transition(2), VertexIndex::position(2));
        let inhibitor_p1_t1 = InhibitorEdge::new(VertexIndex::position(1), VertexIndex::transition(1));
        let inhibitor_p2_t2 = InhibitorEdge::new(VertexIndex::position(2), VertexIndex::transition(2));
        
        edges.add_directed(directed_t1_p1);
        edges.add_directed(directed_t2_p2);
        edges.add_inhibitor(inhibitor_p1_t1);
        edges.add_inhibitor(inhibitor_p2_t2);
        
        edges.remove_relative_to_vertex(VertexIndex::transition(1));
        assert_eq!(edges.directed.len(), 1);
        assert_eq!(edges.directed[0], directed_t2_p2);
        assert_eq!(edges.inhibitor.len(), 1);
        assert_eq!(edges.inhibitor[0], inhibitor_p2_t2);

        edges.remove_relative_to_vertex(VertexIndex::position(2));
        assert_eq!(edges.directed.len(), 0);
        assert_eq!(edges.inhibitor.len(), 0);
    }
} 