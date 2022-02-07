use net::vertex::VertexIndex;
use crate::net::vertex::Vertex;

#[derive(Debug, Clone, Eq, PartialEq)]
pub struct Connection {
    a: VertexIndex,
    b: VertexIndex,
}

impl Connection {
    pub fn new(from: VertexIndex, to: VertexIndex) -> Self {
        Connection { a: from, b: to }
    }

    pub fn first(&self) -> VertexIndex {
        self.a
    }

    pub fn first_mut(&mut self) -> &mut VertexIndex {
        &mut self.a
    }

    pub fn second(&self) -> VertexIndex {
        self.b
    }

    pub fn second_mut(&mut self) -> &mut VertexIndex {
        &mut self.b
    }
}
