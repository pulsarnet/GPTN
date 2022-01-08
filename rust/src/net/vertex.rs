use std::cell::RefCell;
use std::fmt::{Debug, Display, Formatter};
use std::hash::{Hash, Hasher};
use std::rc::Rc;

#[derive(PartialEq, Hash, Eq, Clone)]
pub enum Inner {
    Position(u64, Option<Vertex>),
    Transition(u64, Option<Vertex>)
}

impl Default for Inner {
    fn default() -> Self {
        Inner::Position(0, None)
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
            Inner::Position(i, ..) => format!("p{}", i),
            Inner::Transition(i, ..) => format!("t{}", i)
        };

        f.pad(name.as_str())
    }
}

impl Display for Vertex {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        let name = match *self.0.borrow() {
            Inner::Position(..) => self.full_name(),
            Inner::Transition(..) => self.full_name()
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
        let mut parent = self.clone();
        loop {
            if let Some(p) = parent.get_parent() {
                res = format!("{}.{:?}", res, p);
                parent = p;
                continue
            }
            break
        }

        res
    }

    pub fn index(&self) -> u64 {
        match *self.0.borrow() {
            Inner::Position(i, ..) | Inner::Transition(i, ..) => i
        }
    }

    pub fn set_parent(&mut self, parent: Vertex) {
        match *self.0.borrow_mut() {
            Inner::Position(_, ref mut p) | Inner::Transition(_, ref mut p) => *p = Some(parent),
        }
    }

    pub fn get_parent(&self) -> Option<Self> {
        match *self.0.borrow() {
            Inner::Position(_, Some(ref parent)) | Inner::Transition(_, Some(ref parent)) => Some(parent.clone()),
            _ => None
        }
    }

    pub fn split(&self, new_index: u64) -> Self {
        match *self.0.borrow_mut() {
            Inner::Position(..) => Vertex(Rc::new(RefCell::new(Inner::Position(new_index, Some(self.clone()))))),
            Inner::Transition(..) => Vertex(Rc::new(RefCell::new(Inner::Transition(new_index, Some(self.clone())))))
        }
    }

    pub fn position(index: u64) -> Self {
        Vertex(Rc::new(RefCell::new(Inner::Position(index, None))))
    }

    pub fn transition(index: u64) -> Self {
        Vertex(Rc::new(RefCell::new(Inner::Transition(index, None))))
    }

    pub fn is_position(&self) -> bool {
        match *self.0.borrow() {
            Inner::Position(..) => true,
            _ => false
        }
    }

    pub fn is_transition(&self) -> bool {
        match *self.0.borrow() {
            Inner::Transition(..) => true,
            _ => false
        }
    }
}