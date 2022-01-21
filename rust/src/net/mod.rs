mod connection;
mod vertex;

use core::MatrixFormat;
use nalgebra::DMatrix;
use ndarray_linalg::Solve;
pub use net::connection::Connection;
pub use net::vertex::Vertex;
use std::cell::RefCell;
use std::collections::{HashMap, HashSet};
use std::rc::Rc;
use {SynthesisContext};
use ::{CMatrix, NamedMatrix};

#[derive(Debug)]
pub struct PetriNet {
    pub elements: Vec<Vertex>,
    pub connections: Vec<Connection>,
    position_index: Rc<RefCell<u64>>,
    transition_index: Rc<RefCell<u64>>,
    is_loop: bool,
}

pub struct PetriNetVec(pub Vec<PetriNet>);

impl PetriNetVec {

    pub fn positions(&self) -> Vec<Vertex> {
        self.0
            .iter()
            .flat_map(|n| n.elements.iter().filter(|p| p.is_position()))
            .cloned()
            .collect()
    }

    pub fn transitions(&self) -> Vec<Vertex> {
        self.0
            .iter()
            .flat_map(|n| n.elements.iter().filter(|t| t.is_transition()))
            .cloned()
            .collect()
    }

    pub fn sort(&mut self) {
        self.0.sort_by(|net_a, net_b| {
            net_b
                .elements
                .iter()
                .filter(|e| e.is_position())
                .count()
                .cmp(&net_a.elements.iter().filter(|e| e.is_position()).count())
        });
    }

    pub fn position_indexes(&self) -> HashMap<Vertex, usize> {
        self.0
            .iter()
            .flat_map(|net| net.elements.iter().filter(|e| e.is_position()))
            .enumerate()
            .map(|(i, v)| (v.clone(), i))
            .collect()
    }

    pub fn transition_indexes(&self) -> HashMap<Vertex, usize> {
        self.0
            .iter()
            .flat_map(|net| net.elements.iter().filter(|e| e.is_transition()))
            .enumerate()
            .map(|(i, v)| (v.clone(), i))
            .collect()
    }

    pub fn primitive_net(&self) -> Vec<Vec<Vertex>> {
        let mut result = vec![];

        for net in self.0.iter() {
            let transitions = net
                .elements
                .iter()
                .filter(|t| t.is_transition())
                .cloned()
                .collect::<Vec<_>>();
            'brk: for transition in transitions.into_iter() {
                let mut vertexes = vec![
                    Vertex::position(0),
                    Vertex::transition(0),
                    Vertex::position(0),
                ];
                let mut from = net.connections.iter().filter(|c| c.first().eq(&transition));

                while let Some(f) = from.next() {
                    if result
                        .iter()
                        .flatten()
                        .find(|v: &&Vertex| (*v).eq(f.second()))
                        .is_some()
                    {
                        continue;
                    }

                    let mut to = net
                        .connections
                        .iter()
                        .filter(|c| c.first().ne(f.second()) && c.second().eq(&transition));

                    while let Some(t) = to.next() {
                        if result
                            .iter()
                            .flatten()
                            .find(|v: &&Vertex| (*v).eq(t.first()))
                            .is_some()
                        {
                            continue;
                        }

                        vertexes[0] = t.first().clone();
                        vertexes[1] = f.first().clone();
                        vertexes[2] = f.second().clone();
                        result.push(vertexes);
                        continue 'brk;
                    }
                }
            }
        }

        result
    }


    pub fn primitive_matrix(&self) -> DMatrix<i32> {
        let positions = self.position_indexes();
        let transitions = self.transition_indexes();
        let mut result = DMatrix::<i32>::zeros(positions.len(), transitions.len());

        let primitive_net = self.primitive_net();
        for primitive in primitive_net.into_iter() {
            result.column_mut(*transitions.get(&primitive[1]).unwrap())[*positions.get(&primitive[2]).unwrap()] = 1;
            result.row_mut(*positions.get(&primitive[0]).unwrap())[*transitions.get(&primitive[1]).unwrap()] = -1;
        }

        result
    }
    
    pub fn elements(&self) -> Vec<Vertex> {
        
        self.0
            .iter()
            .flat_map(|n| n.elements.iter())
            .cloned()
            .collect()
        
    }

    pub fn connections(&self) -> Vec<&Connection> {

        self.0
            .iter()
            .flat_map(|n| n.connections.iter())
            .collect()

    }

    fn get_children(el: Vertex, elements: &Vec<Vertex>) -> HashSet<Vertex> {
        let mut result = vec![el.clone()];
        for searched in elements.iter() {
            if let Some(parent) = searched.get_parent() {
                if parent.eq(&el) {
                    result.extend(Self::get_children(searched.clone(), elements).into_iter());
                }
            }
        }
        result.iter().cloned().collect()
    }


    pub fn equivalent_matrix(&self) -> (DMatrix<i32>, DMatrix<i32>) {
        
        let pos_indexes = self.position_indexes();
        let tran_indexes = self.transition_indexes();
        
        let all_elements = self.elements();
        let all_connections = self.connections();

        let mut d_input = nalgebra::DMatrix::<i32>::zeros(pos_indexes.len(), tran_indexes.len());
        let mut d_output = nalgebra::DMatrix::<i32>::zeros(pos_indexes.len(), tran_indexes.len());
        
        for element in all_elements.iter() {
            let mut general = element.clone();
            while let Some(el) = general.get_first_parent() {
                general = el;
            }

            let hierarhy = Self::get_children(general, &all_elements);

            for conn in all_connections
                .iter()
                .filter(|c| hierarhy.contains(c.first()) || hierarhy.contains(c.second()))
            {
                if hierarhy.contains(conn.first()) {
                    for el in hierarhy.iter() {
                        match el.is_transition() {
                            true => {
                                d_output.column_mut(*tran_indexes.get(el).unwrap())
                                    [*pos_indexes.get(conn.second()).unwrap()] = 1;
                            }
                            false => {
                                d_input.row_mut(*pos_indexes.get(el).unwrap())
                                    [*tran_indexes.get(conn.second()).unwrap()] = -1;
                            }
                        }
                    }
                } else if hierarhy.contains(conn.second()) {
                    for el in hierarhy.iter() {
                        match conn.first().is_transition() {
                            true => {
                                d_output.column_mut(*tran_indexes.get(conn.first()).unwrap())
                                    [*pos_indexes.get(el).unwrap()] = 1;
                            }
                            false => {
                                d_input.row_mut(*pos_indexes.get(conn.first()).unwrap())
                                    [*tran_indexes.get(el).unwrap()] = -1;
                            }
                        }
                    }
                }
            }
        }

        (d_input, d_output)
    }

    pub fn lbf_matrix(&self) -> Vec<(NamedMatrix, NamedMatrix)> {
        self.0.iter().map(|net| net.as_matrix()).collect()
    }

}

impl Clone for PetriNet {
    fn clone(&self) -> Self {
        let new_elements = self
            .elements
            .iter()
            .map(|el| el.clone_inner())
            .collect::<Vec<_>>();

        let new_connections = self
            .connections
            .iter()
            .map(|conn| Connection::new(conn.first().clone_inner(), conn.second().clone_inner()))
            .collect::<Vec<_>>();

        PetriNet {
            elements: new_elements,
            connections: new_connections,
            position_index: Rc::new(RefCell::new(*self.position_index.borrow())),
            transition_index: Rc::new(RefCell::new(*self.transition_index.borrow())),
            is_loop: self.is_loop,
        }
    }
}

impl Default for PetriNet {
    fn default() -> Self {
        PetriNet {
            elements: vec![],
            connections: vec![],
            position_index: Rc::new(RefCell::new(1)),
            transition_index: Rc::new(RefCell::new(1)),
            is_loop: false,
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

    pub fn remove_position(&mut self, index: u64) {
        let position = self.get_position(index).cloned().unwrap();
        self.connections
            .drain_filter(|c| c.first().eq(&position) || c.second().eq(&position));
        self.elements
            .remove(self.elements.iter().position(|e| e.eq(&position)).unwrap());
    }

    pub fn get_transition(&self, index: u64) -> Option<&Vertex> {
        self.elements
            .iter()
            .filter(|p| p.is_transition())
            .find(|p| (*p).index().eq(&index))
    }

    pub fn remove_transition(&mut self, index: u64) {
        let transition = self.get_transition(index).cloned().unwrap();
        self.connections
            .drain_filter(|c| c.first().eq(&transition) || c.second().eq(&transition));
        self.elements.remove(
            self.elements
                .iter()
                .position(|e| e.eq(&transition))
                .unwrap(),
        );
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
        if index >= self.get_position_index() {
            *(*self.position_index).borrow_mut() = index + 1;
        }
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
        if index >= self.get_transition_index() {
            *(*self.transition_index).borrow_mut() = index + 1;
        }
        self.elements.last().cloned().unwrap()
    }

    pub fn insert_position(&mut self, element: Vertex, insert: bool) -> Vertex {
        if let Some(p) = self
            .elements
            .iter()
            .filter(|el| el.is_position())
            .find(|el| el.index() == element.index())
            .cloned()
        {
            println!("Element already exists {:?}", p);
            return p;
        }

        if let (Some(found), true) = (
            self.elements
                .iter()
                .enumerate()
                .find(|e| element.get_parent().contains(e.1))
                .map(|e| e.0),
            insert,
        ) {
            self.elements.insert(found + 1, element.clone());
        } else {
            self.elements.push(element.clone());
        }

        if element.index() >= self.get_position_index() {
            *(*self.position_index).borrow_mut() = element.index() + 1;
        }
        element
    }

    pub fn insert_transition(&mut self, element: Vertex) -> Vertex {
        if let Some(p) = self
            .elements
            .iter()
            .filter(|el| el.is_transition())
            .find(|el| el.index() == element.index())
            .cloned()
        {
            println!("Element already exists {:?}", p);
            return p;
        }

        self.elements.push(element.clone());
        if element.index() >= self.get_transition_index() {
            *(*self.transition_index).borrow_mut() = element.index() + 1;
        }
        self.elements.last().cloned().unwrap()
    }

    pub fn insert(&mut self, element: Vertex) -> Vertex {
        match element.is_position() {
            true => self.insert_position(element, false),
            false => self.insert_transition(element),
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

    fn pre_post_arrays(&self) -> (HashMap<Vertex, Vec<Vertex>>, HashMap<Vertex, Vec<Vertex>>) {
        let mut pre = HashMap::<Vertex, Vec<Vertex>>::new();
        let mut post = HashMap::<Vertex, Vec<Vertex>>::new();
        for connection in self.connections.iter() {
            pre.entry(connection.second().clone())
                .or_insert_with(Vec::new)
                .push(connection.first().clone());

            post.entry(connection.first().clone())
                .or_insert_with(Vec::new)
                .push(connection.second().clone());
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

        let mut a_static = nalgebra::DMatrix::<u64>::zeros(b.len(), b.len());
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

            'cont: for (k, _) in ad_dig.iter().enumerate().filter(|(_, e)| **e > 0) {
                let mut x = Vec::new();
                x.resize(i + 1, Vec::new());

                let mut y = Vec::new();
                y.resize(i + 1, Vec::new());

                let mut z = Vec::new();
                z.resize(i + 1, Vec::new());

                x[0] = vec![b[k].clone()];
                y[i] = vec![b[k].clone()];

                let mut j = 1;
                let mut h = i - 1;

                while j <= i {
                    let mut tmp = Vec::new();
                    for el in x[j - 1].iter() {
                        if let Some(v) = post.get(el) {
                            tmp.extend(v.iter().cloned());
                        }
                    }

                    x[j] = tmp;

                    j += 1;

                    let mut tmp = Vec::new();

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
                    let a = x[m].iter().cloned().collect::<HashSet<_>>();
                    let b = y[m].iter().cloned().collect::<HashSet<_>>();
                    let inter = a.intersection(&b).cloned().collect::<HashSet<_>>();
                    z[m] = x[m]
                        .iter()
                        .filter(|e| inter.contains(*e))
                        .cloned()
                        .collect();
                }

                let d = &z[0];
                println!("D -> {:?}", d);
                let mut v = vec![d.iter().next().unwrap().clone()];
                let mut d = v[0].clone();

                for m in (1..i).into_iter() {
                    let a = post
                        .get(&d)
                        .unwrap()
                        .iter()
                        .cloned()
                        .collect::<HashSet<_>>();
                    let b = z[m].iter().cloned().collect::<HashSet<_>>();
                    let inter = a.intersection(&b).cloned().collect::<HashSet<_>>();

                    let intersection = z[m]
                        .iter()
                        .filter(|e| inter.contains(*e))
                        .cloned()
                        .collect::<Vec<_>>();

                    if v.contains(&intersection[0]) {
                        continue 'cont;
                    }
                    v.push(intersection[0].clone());
                    d = v.last().unwrap().clone();
                }

                println!("V -> {:?}", v);
                loops.push(v.clone());
                for part in 2..(i / 2) {
                    let wind = i / part;

                    let s1 = v.iter().take(wind).cloned().collect::<Vec<_>>();
                    let s2 = v.iter().skip(wind).take(wind).cloned().collect::<Vec<_>>();

                    if s1.iter().zip(s2.iter()).filter(|(a, b)| **a == **b).count() == s1.len() {
                        println!("EQ {:?} == {:?}", s1, s2);
                        *loops.last_mut().unwrap() = s1.to_vec();
                        break;
                    }
                }
            }
        }

        loops.sort_by(|a, b| b.len().cmp(&a.len()));
        loops.first().cloned()
    }

    pub fn get_part(&self) -> Option<Vec<Vertex>> {
        let mut result = Vec::new();

        for vertex in self.elements.iter().filter(|v| v.is_position()) {
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

    #[inline]
    pub fn update_position_index(&self) {
        *(*self.position_index).borrow_mut() += 1;
    }

    #[inline]
    pub fn get_position_index(&self) -> u64 {
        *(*self.position_index).borrow()
    }

    pub fn next_position_index(&self) -> u64 {
        let ret = self.get_position_index();
        self.update_position_index();
        ret
    }

    pub fn as_matrix(&self) -> (NamedMatrix, NamedMatrix) {

        let positions = self.elements.iter().filter(|p| p.is_position()).cloned().collect::<Vec<_>>();
        let transitions = self.elements.iter().filter(|p| p.is_transition()).cloned().collect::<Vec<_>>();

        let pos_indexes = positions.iter().enumerate().map(|(i, v)| (v, i)).collect::<HashMap<_, _>>();
        let tran_indexes = transitions.iter().enumerate().map(|(i, v)| (v, i)).collect::<HashMap<_, _>>();

        let mut d_input = DMatrix::<i32>::zeros(positions.len(), transitions.len());
        let mut d_output = DMatrix::<i32>::zeros(positions.len(), transitions.len());

        for conn in self.connections.iter() {
            if conn.first().is_position() {
                d_input.row_mut(*pos_indexes.get(conn.first()).unwrap())[*tran_indexes.get(conn.second()).unwrap()] = -1;
            }
            else {
                d_output.column_mut(*tran_indexes.get(conn.first()).unwrap())[*pos_indexes.get(conn.second()).unwrap()] = 1;
            }
        }

        (NamedMatrix::new_from(positions.clone(), transitions.clone(), d_input),
         NamedMatrix::new_from(positions, transitions, d_output))

    }

    #[inline]
    pub fn update_transition_index(&self) {
        *(*self.transition_index).borrow_mut() += 1;
    }

    #[inline]
    pub fn get_transition_index(&self) -> u64 {
        *self.transition_index.borrow_mut()
    }

    pub fn next_transition_index(&self) -> u64 {
        let ret = self.get_transition_index();
        self.update_transition_index();
        ret
    }

    pub fn normalize(&mut self) {
        let positions = self.elements.iter().filter(|p| p.is_position()).count();
        let transitions = self.elements.iter().filter(|t| t.is_transition()).count();

        if positions == 2 * transitions {
            return;
        }

        // NEED = 2 * T - P
        let mut need = 2 * transitions - positions;
        let mut conns = vec![];
        let mut positions = vec![];

        for position in self.elements.iter().filter(|p| p.is_position()) {
            if need == 0 {
                break;
            }

            let connections = self
                .connections
                .iter()
                .filter(|conn| conn.first().eq(position) || conn.second().eq(position))
                .collect::<Vec<_>>();

            if connections.len() < 2 {
                continue;
            }

            let new_pos = position.split(self.next_position_index());
            positions.push(new_pos.clone());

            for conn in connections.into_iter() {
                match conn.first().is_transition() {
                    true => conns.push(Connection::new(conn.first().clone(), new_pos.clone())),
                    false => conns.push(Connection::new(new_pos.clone(), conn.second().clone())),
                }
            }

            need -= 1;
        }

        positions.into_iter().for_each(|p| {
            self.insert_position(p, true);
        });
        conns.into_iter().for_each(|c| self.connections.push(c));
    }

    pub fn remove_part(&mut self, part: &Vec<Vertex>) -> Self {

        let mut result = PetriNet::new();
        result.position_index = self.position_index.clone();
        result.transition_index = self.transition_index.clone();

        // Fill result with loop elements
        part.iter().for_each(|element| {
            result.insert(element.clone());
        });

        for connection in part.windows(2) {
            let from = &connection[0];
            let to = &connection[1];

            result.connect(from.clone(), to.clone());
            self.connections.drain_filter(|conn| conn.first().eq(from) && conn.second().eq(to));
        }

        if (part.first().unwrap().is_transition() && part.last().unwrap().is_position())
            || (part.first().unwrap().is_position() && part.last().unwrap().is_transition())
        {
            // Это цикл
            result.connect(part.last().unwrap().clone(), part.first().unwrap().clone());
            self.connections.drain_filter(|conn| conn.first().eq(part.last().unwrap()) && conn.second().eq(part.first().unwrap()));
        }

        for loop_element in part.iter() {
            let new_element = match loop_element.is_position() {
                true => loop_element.split(self.get_position_index()),
                false => loop_element.split(self.get_transition_index()),
            };

            let mut add_element = false;
            for connection in self.connections.iter_mut() {
                if connection.first().eq(loop_element) {
                    *connection.first_mut() = new_element.clone();
                    add_element = true;
                }
                else if connection.second().eq(loop_element) {
                    *connection.second_mut() = new_element.clone();
                    add_element = true;
                }
            }

            if add_element {
                match loop_element.is_position() {
                    true => {
                        self.insert_position(new_element.clone(), true);
                        self.update_position_index();
                    },
                    false => {
                        self.insert_transition(new_element.clone());
                        self.update_transition_index();
                    },
                };

                if new_element.is_transition() {
                    if let None = self.connections.iter().find(|c| c.first().eq(&new_element)) {
                        let parent = new_element.get_parent().unwrap();
                        if let Some(mut conn) = result
                            .connections
                            .iter()
                            .find(|c| c.first().eq(&parent))
                            .cloned()
                        {
                            let index = self.next_position_index();
                            let pos = self.insert_position(conn.second().split(index), true);
                            *conn.first_mut() = new_element.clone();
                            *conn.second_mut() = pos.clone();
                            self.connections.push(conn);
                        }
                    }

                    if let None = self
                        .connections
                        .iter()
                        .find(|c| c.second().eq(&new_element))
                    {
                        let parent = new_element.get_parent().unwrap();
                        if let Some(mut conn) = result
                            .connections
                            .iter()
                            .find(|c| c.second().eq(&parent))
                            .cloned()
                        {
                            let index = self.next_position_index();
                            let pos = self.insert_position(conn.first().split(index), true);
                            *conn.first_mut() = pos.clone();
                            *conn.second_mut() = new_element.clone();
                            self.connections.push(conn);
                        }
                    }
                }
            }

            self.elements.remove(self.elements.iter().position(|e| e.eq(loop_element)).unwrap());

        }

        println!("PART RESULT => {result:?}");
        println!("SELF => {self:?}");
        result
    }
}

pub fn transition_synthesis_program(
    t_set: &Vec<usize>,
    d_input: &mut DMatrix<i32>,
    d_output: &mut DMatrix<i32>,
) {
    for t in t_set.iter().skip(1) {
        let res = d_input.column(t_set[0]) + d_input.column(*t);
        d_input.set_column(t_set[0], &res);

        let res = d_output.column(t_set[0]) + d_output.column(*t);
        d_output.set_column(t_set[0], &res);
    }
    for t in t_set.iter().skip(1) {
        let col = d_input.column(t_set[0]).into_owned();
        d_input.set_column(*t, &col);

        let col = d_output.column(t_set[0]).into_owned();
        d_output.set_column(*t, &col);
    }
}

pub fn position_synthesis_program(
    p_set: &Vec<usize>,
    d_input: &mut DMatrix<i32>,
    d_output: &mut DMatrix<i32>,
) {
    for p in p_set.iter().skip(1) {
        let res = d_input.row(p_set[0]) + d_input.row(*p);
        d_input.set_row(p_set[0], &res);

        let res = d_output.row(p_set[0]) + d_output.row(*p);
        d_output.set_row(p_set[0], &res);
    }
    for p in p_set.iter().skip(1) {
        let row = d_input.row(p_set[0]).into_owned();
        d_input.set_row(*p, &row);

        let row = d_output.row(p_set[0]).into_owned();
        d_output.set_row(*p, &row);
    }
}

pub fn synthesis(mut nets: PetriNetVec) -> SynthesisContext {

    nets.sort();

    let pos_indexes = nets.position_indexes();
    let tran_indexes = nets.transition_indexes();

    let positions = pos_indexes.len();
    let transitions = tran_indexes.len();

    let mut c_matrix = nalgebra::DMatrix::<i32>::zeros(positions, positions);
    let primitive_matrix = nets.primitive_matrix();
    let (equivalent_input, equivalent_output) = nets.equivalent_matrix();
    let logical_base_fragments = nets.lbf_matrix();
    

    let poss = nets.positions();
    let transs = nets.transitions();


    let mut d_matrix = nalgebra::DMatrix::<i32>::zeros(positions, transitions);
    for i in 0..d_matrix.nrows() {
        for j in 0..d_matrix.ncols() {
            if (equivalent_input.row(i)[j] + equivalent_output.row(i)[j]) == 0 && equivalent_input.row(i)[j] != 0 {
                d_matrix.row_mut(i)[j] = 0;
            } else {
                d_matrix.row_mut(i)[j] = equivalent_input.row(i)[j] + equivalent_output.row(i)[j];
            }
        }
    }

    let mut entries = vec![];
    let mut result_entries = vec![];

    for i in 0..c_matrix.nrows() {
        let mut entry = ndarray::Array2::zeros((primitive_matrix.nrows(), c_matrix.nrows()));
        let mut result_entry = ndarray::Array1::zeros(c_matrix.nrows());
        for j in 0..primitive_matrix.ncols() {
            for d in 0..primitive_matrix.nrows() {
                entry.row_mut(j)[d] = primitive_matrix.row(d)[j] as f64;
            }
            result_entry[j] = d_matrix.row(i)[j] as f64;
        }

        for j in 0..entry.nrows() {
            for d in 0..entry.ncols() {
                if entry.row(j)[d] == -1.0 {
                    entry.row_mut(j + primitive_matrix.ncols())[d] = 1.0;
                    result_entry[j + primitive_matrix.ncols()] = 0.0; //(rand::random::<u8>() % 4).into();
                                                                //result_entry[j + lbf_matrix.ncols()] = 1.0;//(rand::random::<u8>() % 4).into();
                }
            }
        }

        entries.push(entry);
        result_entries.push(result_entry);
    }

    println!("D: => {}", d_matrix);
    println!("D SIN: => {}", MatrixFormat(&equivalent_input, &transs, &poss));
    println!("D SOUT: => {}", MatrixFormat(&equivalent_output, &transs, &poss));
    println!("PRIMITIVE: => {}", MatrixFormat(&primitive_matrix, &transs, &poss));

    for (index, (a, b)) in entries
        .into_iter()
        .zip(result_entries.into_iter())
        .enumerate()
    {
        let solve = a.solve(&b).unwrap();
        solve
            .into_iter()
            .enumerate()
            .for_each(|v| c_matrix.row_mut(index)[v.0] = v.1 as i32);
    }

    SynthesisContext {
        positions: poss.clone(),
        transitions: transs.clone(),
        programs: vec![],
        c_matrix: CMatrix::from(c_matrix),
        primitive_matrix: CMatrix::from(primitive_matrix),
        //linear_base_fragments: logical_base_fragments,
    }
}
//
// pub fn synthesis_program(programs: &SynthesisProgram, index: usize) -> SynthesisResult {
//     let positions = programs.positions.len();
//     let transitions = programs.transitions.len();
//
//     let mut pos_indexes_vec = programs.positions.iter().map(|p| p.clone_inner()).collect::<Vec<_>>();
//     let mut tran_indexes_vec = programs.transitions.iter().map(|t| t.clone_inner()).collect::<Vec<_>>();
//
//     let c_matrix = programs.c_matrix.clone();
//     let lbf_matrix = programs.lbf_matrix.clone();
//
//     let mut markers = nalgebra::DMatrix::<i32>::zeros(positions, 1);
//     programs
//         .positions
//         .iter()
//         .map(Vertex::markers)
//         .enumerate()
//         .for_each(|e| markers.row_mut(e.0)[0] = e.1 as i32);
//
//     let mut result = nalgebra::DMatrix::<i32>::zeros(positions, transitions);
//     let mut save_vec = nalgebra::DMatrix::<i32>::zeros(positions, transitions);
//
//     let current_program = &programs.programs[index];
//     let mut t_sets = vec![];
//     let mut p_sets = vec![];
//
//     let mut searched = vec![0].into_iter().collect::<HashSet<usize>>();
//     for index_a in 0..tran_indexes_vec.len() {
//         let search_number = current_program[index_a];
//         if searched.contains(&search_number) {
//             continue;
//         }
//
//         let mut indexes = vec![];
//         for index_b in (0..tran_indexes_vec.len()).filter(|e| current_program[*e] == search_number)
//         {
//             if index_a == index_b {
//                 continue;
//             }
//
//             indexes.push(index_b);
//         }
//
//         if indexes.len() > 0 {
//             indexes.push(index_a);
//             t_sets.push(indexes);
//         }
//
//         searched.insert(search_number);
//     }
//
//     let offset = tran_indexes_vec.len();
//     let mut searched = vec![0].into_iter().collect::<HashSet<usize>>();
//     for index_a in offset..(offset + pos_indexes_vec.len()) {
//         let search_number = current_program[index_a];
//         if searched.contains(&search_number) {
//             continue;
//         }
//
//         let mut indexes = vec![];
//         for index_b in (offset..(offset + pos_indexes_vec.len()))
//             .filter(|e| current_program[*e] == search_number)
//         {
//             if index_a == index_b {
//                 continue;
//             }
//
//             indexes.push(index_b - offset);
//         }
//
//         if indexes.len() > 0 {
//             indexes.push(index_a - offset);
//             p_sets.push(indexes);
//         }
//
//         searched.insert(search_number);
//     }
//
//     println!("PSET => {:?}", p_sets);
//     println!("TSET => {:?}", t_sets);
//
//     let mut d_input = nalgebra::DMatrix::<i32>::zeros(positions, transitions);
//     let mut d_output = nalgebra::DMatrix::<i32>::zeros(positions, transitions);
//
//     for i in 0..d_input.nrows() {
//         for j in 0..d_input.ncols() {
//             match lbf_matrix.row(i)[j] {
//                 v if v < 0 => d_input.row_mut(i)[j] = v,
//                 v if v > 0 => d_output.row_mut(i)[j] = v,
//                 _ => {}
//             }
//         }
//     }
//
//     println!(
//         "D INPUT START => {}",
//         MatrixFormat(&d_input, &tran_indexes_vec, &pos_indexes_vec)
//     );
//     println!(
//         "D OUTPUT START => {}",
//         MatrixFormat(&d_output, &tran_indexes_vec, &pos_indexes_vec)
//     );
//
//     for t_set in t_sets.into_iter() {
//         transition_synthesis_program(&t_set, &mut d_input, &mut d_output);
//     }
//
//     for p_set in p_sets.into_iter() {
//         position_synthesis_program(&p_set, &mut d_input, &mut d_output);
//     }
//
//     println!(
//         "D_INPUT => {}",
//         MatrixFormat(&d_input, &tran_indexes_vec, &pos_indexes_vec)
//     );
//     println!(
//         "D_OUTPUT => {}",
//         MatrixFormat(&d_output, &tran_indexes_vec, &pos_indexes_vec)
//     );
//
//     let mut d_matrix = nalgebra::DMatrix::<i32>::zeros(positions, transitions);
//     for i in 0..d_matrix.nrows() {
//         for j in 0..d_matrix.ncols() {
//             if (d_input.row(i)[j] + d_output.row(i)[j]) == 0 && d_input.row(i)[j] != 0 {
//                 d_matrix.row_mut(i)[j] = 0;
//                 save_vec.row_mut(i)[j] = 1;
//             } else {
//                 d_matrix.row_mut(i)[j] = d_input.row(i)[j] + d_output.row(i)[j];
//             }
//         }
//     }
//
//     println!(
//         "D MATRIX => {}",
//         MatrixFormat(&d_matrix, &tran_indexes_vec, &pos_indexes_vec)
//     );
//
//     result = c_matrix.clone() * d_matrix;
//     markers = c_matrix.clone() * markers;
//     println!(
//         "SAVE => {}",
//         MatrixFormat(&save_vec, &tran_indexes_vec, &pos_indexes_vec)
//     );
//     println!(
//         "RESULT => {}",
//         MatrixFormat(&result, &tran_indexes_vec, &pos_indexes_vec)
//     );
//
//     let mut remove_rows = vec![];
//     for (index_a, row_a) in result.row_iter().enumerate() {
//         if result.row(index_a).iter().all(|e| *e == 0)
//             && save_vec.row(index_a).iter().all(|e| *e == 0)
//         {
//             remove_rows.push(index_a);
//             continue;
//         }
//
//         for (index_b, row_b) in result.row_iter().enumerate().skip(index_a) {
//             if index_a == index_b {
//                 continue;
//             }
//
//             if row_a == row_b && save_vec.row(index_a) == save_vec.row(index_b) {
//                 remove_rows.push(index_b);
//
//                 // При объединении эквивалентных позиций выбирается максимальное количество маркеров
//                 markers.row_mut(index_a)[0] = max(
//                     pos_indexes_vec[index_a].markers(),
//                     pos_indexes_vec[index_b].markers(),
//                 ) as i32;
//             }
//         }
//     }
//
//     result = result.remove_rows_at(&remove_rows);
//     d_input = d_input.remove_rows_at(&remove_rows);
//     d_output = d_output.remove_rows_at(&remove_rows);
//     save_vec = save_vec.remove_rows_at(&remove_rows);
//     markers = markers.remove_rows_at(&remove_rows);
//     pos_indexes_vec = pos_indexes_vec
//         .into_iter()
//         .enumerate()
//         .filter(|i| !remove_rows.contains(&i.0))
//         .map(|i| i.1)
//         .collect();
//
//     let mut remove_cols = vec![];
//     for (index_a, column_a) in result.column_iter().enumerate() {
//         if result.column(index_a).iter().all(|e| *e == 0)
//             && save_vec.column(index_a).iter().all(|e| *e == 0)
//         {
//             remove_cols.push(index_a);
//             continue;
//         }
//
//         for (index_b, column_b) in result.column_iter().enumerate().skip(index_a) {
//             if index_a == index_b {
//                 continue;
//             }
//
//             if column_a == column_b && save_vec.column(index_a) == save_vec.column(index_b) {
//                 remove_cols.push(index_b);
//             }
//         }
//     }
//
//     result = result.remove_columns_at(&remove_cols);
//     d_input = d_input.remove_columns_at(&remove_cols);
//     d_output = d_output.remove_columns_at(&remove_cols);
//     save_vec = save_vec.remove_columns_at(&remove_cols);
//     tran_indexes_vec = tran_indexes_vec
//         .into_iter()
//         .enumerate()
//         .filter(|i| !remove_cols.contains(&i.0))
//         .map(|i| i.1)
//         .collect();
//
//     println!(
//         "D_INPUT 2 => {}",
//         MatrixFormat(&d_input, &tran_indexes_vec, &pos_indexes_vec)
//     );
//     println!(
//         "D_OUTPUT 2 => {}",
//         MatrixFormat(&d_output, &tran_indexes_vec, &pos_indexes_vec)
//     );
//     println!(
//         "SAVE 2 => {}",
//         MatrixFormat(&save_vec, &tran_indexes_vec, &pos_indexes_vec)
//     );
//     println!(
//         "RESULT 2 => {}",
//         MatrixFormat(&result, &tran_indexes_vec, &pos_indexes_vec)
//     );
//
//     let mut new_net = PetriNet::new();
//     new_net.elements = pos_indexes_vec
//         .iter()
//         .chain(tran_indexes_vec.iter())
//         .cloned()
//         .collect();
//
//     let mut pos_new_indexes = HashMap::new();
//     for (index, position) in new_net
//         .elements
//         .iter()
//         .filter(|e| e.is_position())
//         .enumerate()
//     {
//         println!("SET MARKERS: {} <= {}", index, markers.row(index)[0]);
//         position.set_markers(markers.row(index)[0] as u64);
//         pos_new_indexes.insert(position.clone(), index);
//     }
//
//     let mut trans_new_indexes = HashMap::new();
//     for (index, transition) in new_net
//         .elements
//         .iter()
//         .filter(|e| e.is_transition())
//         .enumerate()
//     {
//         trans_new_indexes.insert(transition.clone(), index);
//     }
//
//     let mut connections = vec![];
//
//     for transition in new_net.elements.iter().filter(|e| e.is_transition()) {
//         let col = result.column(*trans_new_indexes.get(transition).unwrap());
//         for (index, el) in col.iter().enumerate().filter(|e| e.1.ne(&0)) {
//             let pos = pos_indexes_vec[index].clone();
//             match *el > 0 {
//                 false => connections.push(Connection::new(pos.clone(), transition.clone())),
//                 true => connections.push(Connection::new(transition.clone(), pos.clone())),
//             }
//         }
//     }
//
//     for i in 0..save_vec.nrows() {
//         for j in 0..save_vec.ncols() {
//             if save_vec.row(i)[j] != 1 {
//                 continue;
//             }
//
//             let pos = pos_indexes_vec[i].clone();
//             let tran = tran_indexes_vec[j].clone();
//
//             connections.push(Connection::new(tran.clone(), pos.clone()));
//             connections.push(Connection::new(pos, tran));
//         }
//     }
//
//     new_net.connections = connections;
//
//     SynthesisResult {
//         result_net: new_net,
//         c_matrix,
//         lbf_matrix: NamedMatrix::new_from(programs.positions.clone(), programs.transitions.clone(), programs.lbf_matrix.clone()),
//     }
// }

impl From<Vec<Vertex>> for PetriNet {
    fn from(vertexes: Vec<Vertex>) -> Self {
        let mut result = PetriNet::new();
        for group in vertexes.windows(2) {
            let a = result.insert(group[0].clone());
            let b = result.insert(group[1].clone());

            result.connect(a, b);
        }

        if (vertexes.first().unwrap().is_position() && vertexes.last().unwrap().is_transition())
            || (vertexes.first().unwrap().is_transition() && vertexes.last().unwrap().is_position())
        {
            result.connect(
                vertexes.last().cloned().unwrap(),
                vertexes.first().cloned().unwrap(),
            );
            result.is_loop = true;
        }

        result
    }
}
