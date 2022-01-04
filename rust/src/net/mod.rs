mod connection;
mod vertex;

pub use net::connection::Connection;
pub use net::vertex::Vertex;
use std::collections::{HashMap, HashSet};

#[derive(Debug)]
pub struct PetriNet {
    elements: Vec<Vertex>,
    connections: Vec<Connection>,
    position_index: u64,
    transition_index: u64,
}

impl Default for PetriNet {
    fn default() -> Self {
        PetriNet {
            elements: vec![],
            connections: vec![],
            position_index: 1,
            transition_index: 1,
        }
    }
}

impl PetriNet {
    pub fn new() -> Self {
        PetriNet::default()
    }

    pub fn get_position(&self, index: u64) -> Option<&Vertex> {
        self.elements
            .iter()
            .filter(|p| p.is_position())
            .find(|p| (*p).index().eq(&index))
    }

    pub fn get_transition(&self, index: u64) -> Option<&Vertex> {
        self.elements
            .iter()
            .filter(|p| p.is_transition())
            .find(|p| (*p).index().eq(&index))
    }

    pub fn add_position(&mut self, index: u64) -> Vertex {
        if let Some(p) = self
            .elements
            .iter()
            .filter(|el| el.is_position())
            .find(|el| el.index() == index)
            .cloned()
        {
            return p;
        }

        self.elements.push(Vertex::position(index));
        self.position_index = index + 1;
        self.elements.last().cloned().unwrap()
    }

    pub fn add_transition(&mut self, index: u64) -> Vertex {
        if let Some(p) = self
            .elements
            .iter()
            .filter(|el| el.is_transition())
            .find(|el| el.index() == index)
            .cloned()
        {
            return p;
        }

        self.elements.push(Vertex::transition(index));
        self.transition_index = index + 1;
        self.elements.last().cloned().unwrap()
    }

    pub fn insert_position(&mut self, element: Vertex) -> Vertex {
        if let Some(p) = self
            .elements
            .iter()
            .filter(|el| el.is_position())
            .find(|el| el.index() == element.index())
            .cloned()
        {
            panic!("Element already exists {:?}", p);
        }

        self.elements.push(element.clone());
        self.position_index = element.index() + 1;
        self.elements.last().cloned().unwrap()
    }

    pub fn insert_transition(&mut self, element: Vertex) -> Vertex {
        if let Some(p) = self
            .elements
            .iter()
            .filter(|el| el.is_transition())
            .find(|el| el.index() == element.index())
            .cloned()
        {
            panic!("Element already exists {:?}", p);
        }

        self.elements.push(element.clone());
        self.transition_index = element.index() + 1;
        self.elements.last().cloned().unwrap()
    }

    pub fn insert(&mut self, element: Vertex) -> Vertex {
        match element.is_position() {
            true => self.insert_position(element),
            false => self.insert_transition(element)
        }
    }

    pub fn connect(&mut self, a: Vertex, b: Vertex) {
        if !self.elements.contains(&a) || !self.elements.contains(&b) {
            return;
        }

        self.connections.push(Connection::new(a, b));
    }

    fn split_elements(&self) -> (Vec<Vertex>, Vec<Vertex>) {
        let mut positions = vec![];
        let mut transitions = vec![];

        for element in self.elements.iter().cloned() {
            match element.is_position() {
                false => transitions.push(element),
                true => positions.push(element),
            }
        }

        (positions, transitions)
    }

    fn get_indexes(positions: &[Vertex], transitions: &[Vertex]) -> HashMap<Vertex, usize> {
        transitions
            .iter()
            .chain(positions.iter())
            .cloned()
            .enumerate()
            .fold(HashMap::new(), |mut acc, (index, element)| {
                acc.insert(element, index);
                acc
            })
    }

    fn pre_post_arrays(&self) -> (HashMap<Vertex, HashSet<Vertex>>, HashMap<Vertex, HashSet<Vertex>>) {
        let mut pre = HashMap::<Vertex, HashSet<Vertex>>::new();
        let mut post = HashMap::<Vertex, HashSet<Vertex>>::new();
        for connection in self.connections.iter() {
            pre.entry(connection.second().clone())
                .or_insert_with(HashSet::new)
                .insert(connection.first().clone());

            post.entry(connection.first().clone())
                .or_insert_with(HashSet::new)
                .insert(connection.second().clone());
        }

        (pre, post)
    }

    pub fn get_loop(&self) -> Option<Vec<Vertex>> {
        let mut loops = Vec::<Vec<Vertex>>::new();

        let (positions, transitions) = self.split_elements();
        let indexes = Self::get_indexes(&positions, &transitions);
        let (pre, post) = self.pre_post_arrays();

        let mut b = Vec::new();
        b.resize(indexes.len(), Default::default());
        for index in indexes.iter() {
            b[*index.1] = index.0.clone();
        }

        let mut a_static = nalgebra::DMatrix::<u8>::zeros(b.len(), b.len());
        for (element, children) in pre.iter() {
            let Some(element_index) = indexes.get(element) else { continue };
            for child in children.iter() {
                let Some(child_index) = indexes.get(child) else { continue };
                match element.is_position() {
                    true => a_static.row_mut(*element_index)[*child_index] = 1,
                    false => a_static.column_mut(*child_index)[*element_index] = 1,
                }
            }
        }

        let mut ad = a_static.clone();
        for i in 2..=b.len() {
            ad *= a_static.clone();
            let ad_dig = ad.diagonal();

            for (k, _) in ad_dig.iter().enumerate().filter(|(_, e)| **e > 0) {
                let mut x = Vec::new();
                x.resize(i + 1, HashSet::new());

                let mut y = Vec::new();
                y.resize(i + 1, HashSet::new());

                let mut z = Vec::new();
                z.resize(i + 1, HashSet::new());

                x[0] = vec![b[k].clone()].into_iter().collect();
                y[i] = vec![b[k].clone()].into_iter().collect();

                let mut j = 1;
                let mut h = i - 1;

                while j <= i {
                    let mut tmp = HashSet::new();
                    for el in x[j - 1].iter() {
                        if let Some(v) = post.get(el) {
                            tmp.extend(v.iter().cloned());
                        }
                    }

                    x[j] = tmp;

                    j += 1;

                    let mut tmp = HashSet::new();

                    for el in y[h + 1].iter() {
                        if let Some(v) = pre.get(el) {
                            tmp.extend(v.iter().cloned());
                        }
                    }

                    y[h] = tmp;

                    if h != 0 {
                        h -= 1
                    };
                }

                if !x[i].contains(&b[k]) || !y[0].contains(&b[k]) {
                    continue;
                }

                for m in (0..=i).into_iter() {
                    z[m] = x[m].intersection(&y[m]).cloned().collect();
                }

                let d = &z[0];
                let mut v = vec![d.iter().next().unwrap().clone()];
                let mut d = v[0].clone();

                for m in (1..i).into_iter() {
                    let intersection = post
                        .get(&d)
                        .unwrap()
                        .intersection(&z[m])
                        .cloned()
                        .collect::<Vec<_>>();
                    v.push(intersection[0].clone());
                    d = v.last().unwrap().clone();
                }

                //v.unique();
                if v[0..(i / 2)].to_vec().eq(&v[(i / 2)..i].to_vec()) {
                    println!(
                        "EQ {:?} == {:?}",
                        v[0..(i / 2)].to_vec(),
                        v[(i / 2)..i].to_vec()
                    );
                    loops.push(v[0..(i / 2)].to_vec());
                } else {
                    loops.push(v);
                }
            }
        }

        loops.sort_by(|a, b| b.len().cmp(&a.len()));
        loops.first().cloned()
    }

    pub fn get_part(&self) -> Option<Vec<Vertex>> {
        let mut result = Vec::new();

        for vertex in self
            .elements
            .iter()
            .filter(|v| v.is_position())
        {
            if self
                .connections
                .iter()
                .find(|conn| conn.second().eq(vertex))
                .is_some()
            {
                continue;
            }

            result.push(vertex.clone());

            while let Some(conn) = self
                .connections
                .iter()
                .find(|conn| conn.first() == result.last().unwrap())
            {
                result.push(conn.second().clone());
            }

            break;
        }

        if result.is_empty() {
            None
        } else {
            Some(result)
        }
    }

    pub fn remove_part(&mut self, part: &Vec<Vertex>) {
        let mut new_position = Vertex::position(self.position_index);
        let mut new_transition ;

        for loop_element in part.iter() {
            new_position = Vertex::position(self.position_index);
            new_transition = Vertex::transition(self.transition_index);

            if !self.elements.contains(loop_element) {
                panic!("Element not found: {:?}", loop_element)
            }

            let mut add_element = false;
            for connection in self.connections.iter_mut() {
                if connection.first().eq(loop_element) && !part.contains(connection.second())
                    || (connection.second().eq(loop_element) && !part.contains(connection.first()))
                {
                    match loop_element.is_position() {
                        true if connection.first().eq(loop_element) => {
                            *connection.first_mut() = new_position.clone()
                        }
                        true if connection.second().eq(loop_element) => {
                            *connection.second_mut() = new_position.clone()
                        }
                        false if connection.first().eq(loop_element) => {
                            *connection.first_mut() = new_transition.clone()
                        }
                        false if connection.second().eq(loop_element) => {
                            *connection.second_mut() = new_transition.clone()
                        }
                        _ => unreachable!(),
                    }

                    add_element = true;
                }
            }

            if add_element {
                match loop_element.is_position() {
                    true => self.insert_position(new_position.clone()),
                    false => self.insert_transition(new_transition.clone()),
                };
            }

            self.connections.drain_filter(|conn| {
                conn.first().eq(loop_element) || conn.second().eq(loop_element)
            });
            self.elements.remove(
                self.elements
                    .iter()
                    .position(|el| el.eq(loop_element))
                    .unwrap(),
            );
        }

        println!("RES BEFORE: {:?}", self);

        // Если у перехода нет входа или выхода то надо добавить
        let mut new_self = PetriNet::new();
        for element in self.elements.iter().cloned() {
            let added = new_self.insert(element);

            // Проверяем только переходы
            if added.is_position() {
                continue;
            }

            if let None = self.connections.iter().find(|conn| conn.first().eq(&added)) {
                let vert = new_self.insert_position(new_position.clone());
                new_position = Vertex::position(self.position_index);
                new_self.connect(added, vert)
            } else if let None = self
                .connections
                .iter()
                .find(|conn| conn.second().eq(&added))
            {
                let vert = new_self.insert_position(new_position.clone());
                new_position = Vertex::position(self.position_index);
                new_self.connect(vert, added)
            }
        }

        new_self
            .connections
            .extend(self.connections.iter().cloned());

        *self = new_self;

        println!("RES: {:?}", self);
    }
}
