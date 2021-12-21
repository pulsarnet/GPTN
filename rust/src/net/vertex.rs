use std::fmt::{Debug, Display, Formatter};

#[derive(PartialEq, Hash, Eq, Clone)]
pub enum Vertex {
    Position(u64),
    Transition(u64)
}

impl Debug for Vertex {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        let name = match self {
            Vertex::Position(i) => format!("p{}", i),
            Vertex::Transition(i) => format!("t{}", i)
        };

        f.pad(name.as_str())
    }
}

impl Display for Vertex {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        let name = match self {
            Vertex::Position(i) => format!("p{}", i),
            Vertex::Transition(i) => format!("t{}", i)
        };

        f.pad(name.as_str())
    }
}

impl Default for Vertex {
    fn default() -> Self {
        Vertex::Position(0)
    }
}

impl Vertex {
    pub fn name(&self) -> String {
        format!("{:?}", self)
    }

    pub fn position(index: u64) -> Self {
        Vertex::Position(index)
    }

    pub fn transition(index: u64) -> Self {
        Vertex::Transition(index)
    }

    pub fn is_position(&self) -> bool {
        match self {
            Vertex::Position(_) => true,
            _ => false
        }
    }

    pub fn is_transition(&self) -> bool {
        match self {
            Vertex::Transition(_) => true,
            _ => false
        }
    }
}