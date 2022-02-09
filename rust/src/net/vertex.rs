use std::cell::RefCell;
use std::fmt::{Debug, Display, Formatter};
use std::hash::{Hash, Hasher};
use std::rc::Rc;

#[repr(C)]
#[derive(PartialEq, Hash, Eq, Clone, Copy, Debug)]
pub enum VertexType {
    Position,
    Transition
}

impl Default for VertexType {
    fn default() -> Self {
        VertexType::Position
    }
}

#[repr(C)]
#[derive(Default, PartialEq, Hash, Eq, Clone, Copy, Debug)]
pub struct VertexIndex {
    pub type_: VertexType,
    pub id: usize,
}

impl VertexIndex {
    pub fn position(index: usize) -> VertexIndex {
        VertexIndex {
            type_: VertexType::Position,
            id: index
        }
    }

    pub fn transition(index: usize) -> VertexIndex {
        VertexIndex {
            type_: VertexType::Transition,
            id: index
        }
    }
}

#[derive(Clone, Default)]
pub struct Vertex {
    type_: VertexType,
    id: usize,
    markers: usize,
    parent: Option<VertexIndex>,
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
        let name = format!("{}", self.id);

        f.pad(name.as_str())
    }
}

impl Display for Vertex {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        let name = self.get_name();

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
            res = format!("{}.{}", res, parent.id);
        }
        res
    }

    pub fn markers(&self) -> usize {
        match self.type_ {
            VertexType::Position => self.markers,
            _ => unreachable!(),
        }
    }

    pub fn add_marker(&mut self) {
        match self.type_ {
            VertexType::Position => self.markers += 1,
            _ => unreachable!()
        }
    }

    pub fn set_markers(&mut self, count: usize) {
        match self.type_ {
            VertexType::Position => self.markers = count,
            _ => unreachable!()
        }
    }

    pub fn remove_marker(&mut self) {
        match self.type_ {
            VertexType::Position => self.markers -= 1,
            _ => unreachable!()
        }
    }

    pub fn index(&self) -> VertexIndex {
        VertexIndex {
            type_: self.type_,
            id: self.id
        }
    }

    pub fn get_parent(&self) -> Option<VertexIndex> {
        self.parent
    }

    pub fn set_name(&mut self, name: String) {
        self.name = name;
    }

    pub fn get_name(&self) -> String {
        let mut name = self.name.clone();
        if self.parent.is_some() {
            name = format!("{name}");
        }
        name
    }

    pub fn set_parent(&mut self, p: VertexIndex) {
        self.parent = Some(p);
    }

    pub fn split(&self, new_index: usize) -> Self {
        let mut split = self.clone();
        split.id = new_index;
        split.parent = Some(self.index());
        split
    }

    pub fn position(index: usize) -> Self {
        Vertex {
            type_: VertexType::Position,
            id: index,
            markers: 0,
            parent: None,
            name: String::new()
        }
    }

    pub fn transition(index: usize) -> Self {
        Vertex {
            type_: VertexType::Transition,
            id: index,
            markers: 0,
            parent: None,
            name: String::new()
        }
    }

    pub fn is_position(&self) -> bool {
        match self.type_ {
            VertexType::Position => true,
            _ => false,
        }
    }

    pub fn is_transition(&self) -> bool {
        match self.type_ {
            VertexType::Transition => true,
            _ => false,
        }
    }
}
