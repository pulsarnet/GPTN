use net::vertex::VertexIndex;

#[derive(Debug, Copy, Clone, PartialEq, Eq)]
pub struct DirectedEdge {
    begin: VertexIndex,
    end: VertexIndex,
    weight: u32
}


impl DirectedEdge {
    pub fn new(begin: VertexIndex, end: VertexIndex) -> Self {
        Self {
            begin,
            end,
            weight: 1,
        }
    }

    pub fn new_with(begin: VertexIndex, end: VertexIndex, weight: u32) -> Self {
        Self {
            begin,
            end,
            weight,
        }
    }
    
    pub fn begin(&self) -> VertexIndex {
        self.begin
    }
    
    pub fn end(&self) -> VertexIndex {
        self.end
    }
    
    pub fn weight(&self) -> u32 {
        self.weight
    }
    
    pub fn set_weight(&mut self, weight: u32) {
        self.weight = weight
    }
}