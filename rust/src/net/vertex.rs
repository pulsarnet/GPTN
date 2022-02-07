use std::cell::RefCell;
use std::fmt::{Debug, Display, Formatter};
use std::hash::{Hash, Hasher};
use std::rc::Rc;

#[derive(PartialEq, Hash, Eq, Clone)]
pub enum VertexType {
    Position(u64, u64),
    Transition(u64),
}

impl Default for VertexType {
    fn default() -> Self {
        VertexType::Position(0, 0)
    }
}

#[derive(Clone, Default)]
pub struct Vertex {
    type_: VertexType,
    parent: Option<u64>,
    name: String
}

impl Hash for Vertex {
    fn hash<H: Hasher>(&self, state: &mut H) {
        self.type_.hash(state)
    }
}

impl PartialEq for Vertex {
    fn eq(&self, other: &Self) -> bool {
        self.type_.eq(&other.type_)
    }
}

impl Eq for Vertex {}

impl Debug for Vertex {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        let name = match self.type_ {
            VertexType::Position(i, ..) => format!("p{}", i),
            VertexType::Transition(i, ..) => format!("t{}", i),
        };

        f.pad(name.as_str())
    }
}

impl Display for Vertex {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        let name = match self.type_ {
            VertexType::Position(..) => self.get_name(),
            VertexType::Transition(..) => self.get_name(),
        };

        f.pad(name.as_str())
    }
}

impl Vertex {

    pub fn name(&self) -> String {
        format!("{:?}", self)
    }

    pub fn full_name(&self) -> String {
        let mut res = format!("{:?}", self);
        if let Some(parent) = self.parent {
            res = format!("{}.{}", res, parent);
        }
        res
    }

    pub fn markers(&self) -> u64 {
        match self.type_ {
            VertexType::Position(_, markers) => markers,
            _ => unreachable!(),
        }
    }

    pub fn add_marker(&mut self) {
        if let VertexType::Position(.., ref mut v) = self.type_ {
            *v += 1;
        }
    }

    pub fn set_markers(&mut self, count: u64) {
        if let VertexType::Position(.., ref mut v) = self.type_ {
            *v = count;
        }
    }

    pub fn remove_marker(&mut self) {
        if let VertexType::Position(.., ref mut v) = self.type_ {
            if *v > 0 {
                *v -= 1;
            }
        }
    }

    pub fn index(&self) -> u64 {
        match self.type_ {
            VertexType::Position(i, ..) | VertexType::Transition(i, ..) => i,
        }
    }

    pub fn get_parent(&self) -> Option<u64> {
        self.parent
    }

    pub fn set_name(&mut self, name: String) {
        self.name = name;
    }

    pub fn get_name(&self) -> String {
        let mut name = self.name.clone();
        if self.parent.is_some() {
            name = format!("{name}'");
        }
        name
    }

    pub fn set_parent(&mut self, p: u64) {
        self.parent = Some(p);
    }

    pub fn split(&self, new_index: u64) -> Self {
        let mut split = self.clone();
        match split.type_ {
            VertexType::Position(ref mut index, _) | VertexType::Transition(ref mut index) => *index = new_index
        };

        split.parent = Some(self.index());
        split
    }

    pub fn position(index: u64) -> Self {
        Vertex {
            type_: VertexType::Position(index, 0),
            parent: None,
            name: String::new()
        }
    }

    pub fn transition(index: u64) -> Self {
        Vertex {
            type_: VertexType::Transition(index),
            parent: None,
            name: String::new()
        }
    }

    pub fn is_position(&self) -> bool {
        match self.type_ {
            VertexType::Position(..) => true,
            _ => false,
        }
    }

    pub fn is_transition(&self) -> bool {
        match self.type_ {
            VertexType::Transition(..) => true,
            _ => false,
        }
    }
}
