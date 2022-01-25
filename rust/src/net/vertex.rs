use std::cell::RefCell;
use std::fmt::{Debug, Display, Formatter};
use std::hash::{Hash, Hasher};
use std::rc::Rc;

#[derive(PartialEq, Hash, Eq, Clone)]
pub enum VertexType {
    Position(u64, u64),
    Transition(u64),
}

#[derive(Clone, Default)]
pub struct Inner {
    t: VertexType,
    p: Option<Vertex>
}

impl Default for VertexType {
    fn default() -> Self {
        VertexType::Position(0, 0)
    }
}

#[derive(Clone, Default)]
pub struct Vertex(Rc<RefCell<Inner>>);

impl Hash for Vertex {
    fn hash<H: Hasher>(&self, state: &mut H) {
        (*self.0.borrow()).t.hash(state);
    }
}

impl PartialEq for Vertex {
    fn eq(&self, other: &Self) -> bool {
        ((*self.0.borrow()).t).eq(&(*other.0.borrow()).t)
    }
}

impl Eq for Vertex {}

impl Debug for Vertex {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        let name = match (*self.0.borrow()).t {
            VertexType::Position(i, ..) => format!("p{}", i),
            VertexType::Transition(i, ..) => format!("t{}", i),
        };

        f.pad(name.as_str())
    }
}

impl Display for Vertex {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        let name = match (*self.0.borrow()).t {
            VertexType::Position(..) => self.full_name(),
            VertexType::Transition(..) => self.full_name(),
        };

        f.pad(name.as_str())
    }
}

impl Vertex {
    pub fn clone_inner(&self) -> Self {
        let cloned = Vertex(Rc::new(RefCell::new((*self.0.borrow()).clone())));
        if let Some(p) = cloned.get_parent() {
            cloned.set_parent(p.clone_inner());
        }
        cloned
    }

    pub fn name(&self) -> String {
        format!("{:?}", self)
    }

    pub fn full_name(&self) -> String {
        let mut res = format!("{:?}", self);
        if let Some(parent) = self.get_first_parent() {
            res = format!("{}.{}", res, parent.index());
        }
        res
    }

    pub fn markers(&self) -> u64 {
        match (*self.0.borrow_mut()).t {
            VertexType::Position(_, markers) => markers,
            _ => unreachable!(),
        }
    }

    pub fn add_marker(&self) {
        if let VertexType::Position(.., ref mut v) = (*self.0.borrow_mut()).t {
            *v += 1;
        }
    }

    pub fn set_markers(&self, count: u64) {
        if let VertexType::Position(.., ref mut v) = (*self.0.borrow_mut()).t {
            *v += count;
        }
    }

    pub fn remove_marker(&self) {
        if let VertexType::Position(.., ref mut v) = (*self.0.borrow_mut()).t {
            if *v > 0 {
                *v -= 1;
            }
        }
    }

    pub fn index(&self) -> u64 {
        match (*self.0.borrow_mut()).t {
            VertexType::Position(i, ..) | VertexType::Transition(i, ..) => i,
        }
    }

    pub fn get_parent(&self) -> Option<Self> {
        (*self.0.borrow()).p.clone()
    }

    pub fn get_first_parent(&self) -> Option<Self> {
        let mut result = None;

        if let Some(t) = self.get_parent() {
            match t.get_first_parent() {
                Some(v) => result = Some(v),
                None => result = Some(t)
            }
        }

        result
    }

    pub fn set_parent(&self, p: Vertex) {
        (*self.0.borrow_mut()).p = Some(p);
    }

    pub fn split(&self, new_index: u64) -> Self {
        let split = self.clone_inner();
        match split.0.borrow_mut().t {
            VertexType::Position(ref mut index, _) | VertexType::Transition(ref mut index) => *index = new_index
        };

        (*split.0.borrow_mut()).p = Some(self.clone_inner());
        split
    }

    pub fn position(index: u64) -> Self {
        Vertex(Rc::new(RefCell::new(
            Inner {
                t: VertexType::Position(index, 0),
                p: None
            }
        )))
    }

    pub fn transition(index: u64) -> Self {
        Vertex(Rc::new(RefCell::new(
            Inner {
                t: VertexType::Transition(index),
                p: None
            }
        )))
    }

    pub fn is_position(&self) -> bool {
        match (*self.0.borrow_mut()).t {
            VertexType::Position(..) => true,
            _ => false,
        }
    }

    pub fn is_transition(&self) -> bool {
        match (*self.0.borrow_mut()).t {
            VertexType::Transition(..) => true,
            _ => false,
        }
    }
}
