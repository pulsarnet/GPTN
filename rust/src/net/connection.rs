use crate::net::vertex::Vertex;

#[derive(Debug, Clone, Eq, PartialEq)]
pub struct Connection {
    a: Vertex,
    b: Vertex,
}

impl Connection {
    pub fn new(from: Vertex, to: Vertex) -> Self {
        Connection { a: from, b: to }
    }

    pub fn first(&self) -> &Vertex {
        &self.a
    }

    pub fn first_mut(&mut self) -> &mut Vertex {
        &mut self.a
    }

    pub fn second(&self) -> &Vertex {
        &self.b
    }

    pub fn second_mut(&mut self) -> &mut Vertex {
        &mut self.b
    }
}
