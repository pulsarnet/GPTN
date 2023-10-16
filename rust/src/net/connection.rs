use net::vertex::VertexIndex;

#[derive(Debug, Clone, Eq, PartialEq, Copy)]
pub struct Connection {
    from: VertexIndex,
    to: VertexIndex,
    weight: usize,
}

impl Connection {
    pub fn new(from: VertexIndex, to: VertexIndex, weight: usize) -> Self {
        Connection {
            from,
            to,
            weight,
        }
    }

    pub fn first(&self) -> VertexIndex {
        self.from
    }

    pub fn first_mut(&mut self) -> &mut VertexIndex {
        &mut self.from
    }

    pub fn second(&self) -> VertexIndex {
        self.to
    }

    pub fn second_mut(&mut self) -> &mut VertexIndex {
        &mut self.to
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
