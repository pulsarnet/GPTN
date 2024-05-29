use net::vertex::{VertexIndex, VertexType};

#[derive(Debug, Copy, Clone, PartialEq, Eq)]
pub struct InhibitorEdge {
    place: VertexIndex,
    transition: VertexIndex,
}


impl InhibitorEdge {
    pub fn new(place: VertexIndex, transition: VertexIndex) -> Self {
        assert_eq!(place.type_, VertexType::Position);
        assert_eq!(transition.type_, VertexType::Transition);
        
        Self {
            place,
            transition
        }
    }

    pub fn place(&self) -> VertexIndex {
        self.place
    }

    pub fn transition(&self) -> VertexIndex {
        self.transition
    }
}