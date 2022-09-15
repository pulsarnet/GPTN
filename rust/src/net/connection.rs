use net::vertex::VertexIndex;

#[derive(Debug, Clone, Eq, PartialEq)]
pub struct Connection {
    a: VertexIndex,
    b: VertexIndex,
    weight: usize,
}

impl Connection {
    pub fn new(from: VertexIndex, to: VertexIndex) -> Self {
        Connection {
            a: from,
            b: to,
            weight: 1,
        }
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

    pub fn increment(&mut self) {
        self.weight += 1;
    }

    pub fn decrement(&mut self) {
        self.weight = self.weight.saturating_sub(1)
    }

    pub fn set_weight(&mut self, weight: usize) {
        self.weight = weight;
    }

    pub fn weight(&self) -> usize {
        self.weight
    }
}
