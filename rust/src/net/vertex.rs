use std::cell::RefCell;
use std::fmt::{Debug, Display, Formatter};
use std::hash::{Hash, Hasher};
use std::rc::Rc;

#[derive(PartialEq, Hash, Eq, Clone)]
pub enum Inner {
    Position(u64),
    Transition(u64)
}

impl Default for Inner {
    fn default() -> Self {
        Inner::Position(0)
    }
}

#[derive(Clone, Default)]
pub struct Vertex(Rc<RefCell<Inner>>);

impl Hash for Vertex {
    fn hash<H: Hasher>(&self, state: &mut H) {
        (*self.0.borrow()).hash(state);
    }
}

impl PartialEq for Vertex {
    fn eq(&self, other: &Self) -> bool {
        (*self.0.borrow()).eq(&*other.0.borrow())
    }
}

impl Eq for Vertex {

}

impl Debug for Vertex {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        let name = match *self.0.borrow() {
            Inner::Position(i) => format!("p{}", i),
            Inner::Transition(i) => format!("t{}", i)
        };

        f.pad(name.as_str())
    }
}

impl Display for Vertex {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        let name = match *self.0.borrow() {
            Inner::Position(i) => format!("p{}", i),
            Inner::Transition(i) => format!("t{}", i)
        };

        f.pad(name.as_str())
    }
}

impl Vertex {
    pub fn name(&self) -> String {
        format!("{:?}", self)
    }

    pub fn index(&self) -> u64 {
        match *self.0.borrow() {
            Inner::Position(i) | Inner::Transition(i) => i
        }
    }

    pub fn position(index: u64) -> Self {
        Vertex(Rc::new(RefCell::new(Inner::Position(index))))
    }

    pub fn transition(index: u64) -> Self {
        Vertex(Rc::new(RefCell::new(Inner::Transition(index))))
    }

    pub fn is_position(&self) -> bool {
        match *self.0.borrow() {
            Inner::Position(_) => true,
            _ => false
        }
    }

    pub fn is_transition(&self) -> bool {
        match *self.0.borrow() {
            Inner::Transition(_) => true,
            _ => false
        }
    }
}