mod connection;
pub mod vertex;

use core::MatrixFormat;
use nalgebra::DMatrix;

pub use net::connection::Connection;
pub use net::vertex::Vertex;
use std::cell::RefCell;
use std::cmp::max;
use std::collections::{HashMap, HashSet};
use std::ops::Index;
use std::rc::Rc;

use ::{DecomposeContext};
use net::vertex::{VertexIndex, VertexType};
use indexmap::map::IndexMap;
use crate::ffi::matrix::CNamedMatrix;

#[derive(Debug)]
pub struct PetriNet {
    pub positions: IndexMap<VertexIndex, Vertex>,
    pub transitions: IndexMap<VertexIndex, Vertex>,
    pub connections: Vec<Connection>,
    position_index: Rc<RefCell<usize>>,
    transition_index: Rc<RefCell<usize>>,
    is_loop: bool,
}

pub struct PetriNetVec(pub Vec<PetriNet>);

impl PetriNetVec {

    pub fn positions(&self) -> Vec<Vertex> {
        self.0
            .iter()
            .flat_map(|n| n.positions.iter().map(|p| p.1))
            .cloned()
            .collect()
    }

    pub fn transitions(&self) -> Vec<Vertex> {
        self.0
            .iter()
            .flat_map(|n| n.transitions.iter().map(|p| p.1))
            .cloned()
            .collect()
    }

    pub fn sort(&mut self) {
        self.0.sort_by(|net_a, net_b| {
            net_b.positions.len().cmp(&net_a.positions.len())
        });
    }

    pub fn position_indexes(&self) -> HashMap<VertexIndex, usize> {
        self.0
            .iter()
            .flat_map(|net| net.positions.keys())
            .enumerate()
            .map(|(i, &v)| (v, i))
            .collect()
    }

    pub fn transition_indexes(&self) -> HashMap<VertexIndex, usize> {
        self.0
            .iter()
            .flat_map(|net| net.transitions.keys())
            .enumerate()
            .map(|(i, &v)| (v, i))
            .collect()
    }

    pub fn primitive_net(&self) -> PetriNet {
        let mut result = PetriNet::new();

        for net in self.0.iter() {
            let transitions = &net.transitions;

            'brk: for transition in transitions.values() {
                let mut from = net.connections.iter().filter(|c| c.first().eq(&transition.index()));

                while let Some(f) = from.next() {
                    if result.positions.get(&f.second()).is_some() {
                        continue;
                    }

                    let mut to = net
                        .connections
                        .iter()
                        .filter(|c| c.first().ne(&f.second()) && c.second().eq(&transition.index()));

                    while let Some(t) = to.next() {
                        if result.positions.get(&t.first()).is_some() {
                            continue;
                        }

                        result.insert(net.get_position(t.first().id).unwrap().clone());
                        result.insert(net.get_transition(f.first().id).unwrap().clone());
                        result.insert(net.get_position(f.second().id).unwrap().clone());

                        // TODO: Как соединять веса
                        result.connect(t.first(), f.first().clone());
                        result.connect(f.first(), f.second().clone());
                        continue 'brk;
                    }
                }
            }
        }

        result
    }

    /// Возвращает матрицы D(I) и D(O) для примитивной системы
    /// Элементы данных матриц, это положительные числа
    pub fn primitive_matrix(&self) -> (DMatrix<i32>, DMatrix<i32>) {
        let positions = self.position_indexes();
        let transitions = self.transition_indexes();
        let mut d_input = DMatrix::<i32>::zeros(positions.len(), transitions.len());
        let mut d_output = DMatrix::<i32>::zeros(positions.len(), transitions.len());

        let primitive_net = self.primitive_net();
        for connection in primitive_net.connections.iter() {
            match connection.first().type_ {
                VertexType::Transition => {
                    d_output.column_mut(*transitions.get(&connection.first()).unwrap())[*positions.get(&connection.second()).unwrap()] = connection.weight() as i32;
                },
                _ => {
                    d_input.row_mut(*positions.get(&connection.first()).unwrap())[*transitions.get(&connection.second()).unwrap()] = connection.weight() as i32;
                }
            }
        }

        (d_input, d_output)
    }
    
    pub fn elements(&self) -> Vec<Vertex> {
        self.0
            .iter()
            .flat_map(|n| n.positions.values().chain(n.transitions.values()))
            .cloned()
            .collect()
        
    }

    pub fn connections(&self) -> Vec<&Connection> {
        self.0
            .iter()
            .flat_map(|n| n.connections.iter())
            .collect()
    }

    /// Возвращает матрицы D(I) и D(O) для системы ЛБФ
    /// Элементы данных матриц, это положительные числа
    pub fn equivalent_matrix(&self) -> (DMatrix<i32>, DMatrix<i32>) {
        let all_connections = self.connections();
        let pos_indexes = self.position_indexes();
        let tran_indexes = self.transition_indexes();

        let mut d_input = nalgebra::DMatrix::<i32>::zeros(pos_indexes.len(), tran_indexes.len());
        let mut d_output = nalgebra::DMatrix::<i32>::zeros(pos_indexes.len(), tran_indexes.len());

        for connection in all_connections {
            let first = connection.first();
            let second = connection.second();

            match first.type_ {
                VertexType::Transition => {
                    d_output.column_mut(*tran_indexes.get(&first).unwrap())[*pos_indexes.get(&second).unwrap()] = connection.weight() as i32;
                },
                _ => {
                    d_input.row_mut(*pos_indexes.get(&first).unwrap())[*tran_indexes.get(&second).unwrap()] = connection.weight() as i32;
                }
            }
        }

        (d_input,d_output)
    }

    pub fn lbf_matrix(&self) -> Vec<(CNamedMatrix, CNamedMatrix)> {
        self.0.iter().map(|net| net.as_matrix()).collect()
    }

}

impl Clone for PetriNet {
    fn clone(&self) -> Self {
        let new_connections = self
            .connections
            .iter()
            .map(|conn| {
                let mut connect = Connection::new(conn.first().clone(), conn.second().clone());
                connect.set_weight(conn.weight());
                connect
            })
            .collect::<Vec<_>>();

        PetriNet {
            positions: self.positions.clone(),
            transitions: self.transitions.clone(),
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
            positions: IndexMap::new(),
            transitions: IndexMap::new(),
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

    pub fn output_positions(&self) -> usize {
        let mut post_c = 0;
        for pos in self.positions.keys() {
            let mut filter = self.connections.iter()
                .filter(|conn| conn.first() == *pos);

            if filter.next().is_none() {
                post_c += 1;
            }
        }
        post_c
    }

    pub fn input_positions(&self) -> usize {
        let mut pre_c = 0;
        for pos in self.positions.keys() {
            let mut  filter = self.connections.iter()
                .filter(|conn| conn.second() == *pos);

            if filter.next().is_none() {
                pre_c += 1;
            }
        }
        pre_c
    }

    pub fn get_position(&self, index: usize) -> Option<&Vertex> {
        self.positions.get(&VertexIndex::position(index))
    }

    pub fn remove_position(&mut self, index: usize) {
        let position = self.get_position(index).unwrap().index();
        self.connections
            .drain_filter(|c| c.first().eq(&position) || c.first().eq(&position));
        self.positions.remove(&position);
    }

    pub fn get_transition(&self, index: usize) -> Option<&Vertex> {
        self.transitions.get(&VertexIndex::transition(index))
    }

    pub fn remove_transition(&mut self, index: usize) {
        let transition = self.get_transition(index).unwrap().index();
        self.connections
            .drain_filter(|c| c.first().eq(&transition) || c.second().eq(&transition));
        self.transitions.remove(&transition);
    }

    pub fn add_position(&mut self, index: usize) -> &Vertex {
        self.positions
            .entry(VertexIndex::position(index))
            .or_insert_with(|| Vertex::position(index));

        if index >= self.get_position_index() {
            *(*self.position_index).borrow_mut() = index + 1;
        }
        self.positions.get(&VertexIndex::position(index)).unwrap()
    }

    pub fn add_transition(&mut self, index: usize) -> &Vertex {
        self.transitions
            .entry(VertexIndex::transition(index))
            .or_insert_with(|| Vertex::transition(index));

        if index >= self.get_transition_index() {
            *(*self.transition_index).borrow_mut() = index + 1;
        }
        self.transitions.get(&VertexIndex::transition(index)).unwrap()
    }

    pub fn insert_position(&mut self, element: Vertex) -> &Vertex {
        match self.positions.contains_key(&element.index()) {
            true => self.positions.get(&element.index()).unwrap(),
            false => {
                let index = element.index();
                if index.id >= self.get_position_index() {
                    *(*self.position_index).borrow_mut() = index.id + 1;
                }

                if let Some(parent_index) = element.get_parent() {
                    // Найдем индекс родителя
                    match self.positions.iter().position(|(pos_index, _)| *pos_index == parent_index) {
                        Some(i) => {
                            let new_map = self.positions.split_off(i + 1);
                            self.positions.insert(index, element);
                            self.positions.extend(new_map.into_iter());
                        },
                        None => { self.positions.insert(index, element); },
                    }
                }
                else {
                    self.positions.insert(index,element);
                }
                self.positions.get(&index).unwrap()
            }
        }
    }

    pub fn insert_transition(&mut self, element: Vertex) -> &Vertex {
        match self.transitions.contains_key(&element.index()) {
            true => self.transitions.get(&element.index()).unwrap(),
            false => {
                let index = element.index();
                if index.id >= self.get_transition_index() {
                    *(*self.transition_index).borrow_mut() = index.id + 1;
                }

                if let Some(parent_index) = element.get_parent() {
                    // Найдем индекс родителя
                    match self.transitions.iter().position(|(tran_index, _)| *tran_index == parent_index) {
                        Some(i) => {
                            let new_map = self.transitions.split_off(i + 1);
                            self.transitions.insert(index, element);
                            self.transitions.extend(new_map.into_iter());
                        }
                        None => { self.transitions.insert(index, element); },
                    }
                }
                else {
                    self.transitions.insert(index, element);
                }
                self.transitions.get(&index).unwrap()
            }
        }
    }

    pub fn insert(&mut self, element: Vertex) -> &Vertex {
        match element.is_position() {
            true => self.insert_position(element),
            false => self.insert_transition(element),
        }
    }

    pub fn connect(&mut self, a: VertexIndex, b: VertexIndex) {
        match self.connections.iter_mut()
            .find(|c| c.first() == a && c.second() == b)
        {
            Some(connection) => connection.increment(),
            None => self.connections.push(Connection::new(a, b)),
        }
    }

    fn get_indexes(positions: &IndexMap<VertexIndex, Vertex>, transitions: &IndexMap<VertexIndex, Vertex>) -> HashMap<VertexIndex, usize> {
        transitions
            .values()
            .chain(positions.values())
            .enumerate()
            .fold(HashMap::new(), |mut acc, (index, element)| {
                acc.insert(element.index(), index);
                acc
            })
    }

    fn pre_post_arrays(&self) -> (HashMap<VertexIndex, Vec<VertexIndex>>, HashMap<VertexIndex, Vec<VertexIndex>>) {
        let mut pre = HashMap::<VertexIndex, Vec<VertexIndex>>::new();
        let mut post = HashMap::<VertexIndex, Vec<VertexIndex>>::new();
        for connection in self.connections.iter() {
            pre.entry(connection.second())
                .or_insert_with(Vec::new)
                .push(connection.first());

            post.entry(connection.first().clone())
                .or_insert_with(Vec::new)
                .push(connection.second());
        }

        (pre, post)
    }

    pub fn get_loop(&self) -> Option<Vec<VertexIndex>> {
        let mut loops = Vec::<Vec<VertexIndex>>::new();

        let (positions, transitions) = (&self.positions, &self.transitions);
        let indexes = Self::get_indexes(&positions, &transitions);
        let (pre, post) = self.pre_post_arrays();

        let mut b = Vec::new();
        b.resize(indexes.len(), Default::default());
        for index in indexes.iter() {
            b[*index.1] = index.0.clone();
        }

        let mut a_static = nalgebra::DMatrix::<u64>::zeros(b.len(), b.len());
        for (element, children) in pre.iter() {
            let Some(&element_index) = indexes.get(element) else { continue };
            for child in children.iter() {
                let Some(&child_index) = indexes.get(child) else { continue };
                match element.type_ {
                    VertexType::Position => a_static.row_mut(element_index)[child_index] = 1,
                    VertexType::Transition => a_static.column_mut(child_index)[element_index] = 1,
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
                log::info!("D -> {:?}", d);
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

                log::info!("V -> {:?}", v);
                loops.push(v.clone());
                for part in 2..(i / 2) {
                    let wind = i / part;

                    let s1 = v.iter().take(wind).cloned().collect::<Vec<_>>();
                    let s2 = v.iter().skip(wind).take(wind).cloned().collect::<Vec<_>>();

                    if s1.iter().zip(s2.iter()).filter(|(a, b)| **a == **b).count() == s1.len() {
                        log::info!("EQ {:?} == {:?}", s1, s2);
                        *loops.last_mut().unwrap() = s1.to_vec();
                        break;
                    }
                }
            }
        }

        loops.sort_by(|a, b| b.len().cmp(&a.len()));
        loops.first().cloned()
    }

    pub fn get_part(&self) -> Option<Vec<VertexIndex>> {
        let mut result = Vec::new();

        for vertex in self.positions.values() {

            // If incoming edge to vertex exists then skip
            if self
                .connections
                .iter()
                .find(|conn| conn.second().eq(&vertex.index()))
                .is_some()
            {
                continue;
            }

            result.push(vertex.index());

            while let Some(conn) = self
                .connections
                .iter()
                .find(|conn| conn.first() == *result.last().unwrap())
            {
                result.push(conn.second());
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
    pub fn get_position_index(&self) -> usize {
        *(*self.position_index).borrow()
    }

    pub fn next_position_index(&self) -> usize {
        let ret = self.get_position_index();
        self.update_position_index();
        ret
    }

    pub fn as_matrix(&self) -> (CNamedMatrix, CNamedMatrix) {

        let positions = self.positions.clone();
        let transitions = self.transitions.clone();

        let pos_indexes = positions.values().enumerate().map(|(i, v)| (v.index(), i)).collect::<HashMap<_, _>>();
        let tran_indexes = transitions.values().enumerate().map(|(i, v)| (v.index(), i)).collect::<HashMap<_, _>>();

        let mut d_input = DMatrix::<i32>::zeros(positions.len(), transitions.len());
        let mut d_output = DMatrix::<i32>::zeros(positions.len(), transitions.len());

        for conn in self.connections.iter() {
            match conn.first().type_ {
                VertexType::Position => d_input.row_mut(*pos_indexes.get(&conn.first()).unwrap())[*tran_indexes.get(&conn.second()).unwrap()] = -(conn.weight() as i32),
                VertexType::Transition => d_output.column_mut(*tran_indexes.get(&conn.first()).unwrap())[*pos_indexes.get(&conn.second()).unwrap()] = conn.weight() as i32
            }
        }

        (CNamedMatrix::new(&positions, &transitions, d_input),
         CNamedMatrix::new(&positions, &transitions, d_output))

    }

    pub fn incidence_matrix(&self) -> (CNamedMatrix, CNamedMatrix) {
        let positions = self.positions.values().collect::<Vec<_>>();
        let transitions = self.transitions.values().collect::<Vec<_>>();

        let position_indexes = positions.iter().enumerate().map(|(i, v)| (v.index(), i)).collect::<HashMap<_, _>>();
        let transition_indexes = transitions.iter().enumerate().map(|(i, v)| (v.index(), i)).collect::<HashMap<_, _>>();

        let mut input = DMatrix::<i32>::zeros(positions.len(), transitions.len());
        let mut output = DMatrix::<i32>::zeros(positions.len(), transitions.len());

        for conn in self.connections.iter() {
            match conn.first().type_ {
                VertexType::Transition => output.row_mut(*position_indexes.get(&conn.second()).unwrap())[*transition_indexes.get(&conn.first()).unwrap()] = conn.weight() as i32,
                VertexType::Position => input.row_mut(*position_indexes.get(&conn.first()).unwrap())[*transition_indexes.get(&conn.second()).unwrap()] = conn.weight() as i32,
            }
        }

        (
            CNamedMatrix::new(&self.positions, &self.transitions, input),
            CNamedMatrix::new(&self.positions, &self.transitions, output)
        )
    }

    pub fn adjacency_matrix(&self) -> DMatrix<f64> {
        let mut adjacency = DMatrix::<f64>::zeros(self.positions.len(), self.positions.len());

        for conn in self.connections.iter() {
            match conn.first().type_ {
                VertexType::Transition => {
                    let place_index = self.positions.get_index_of(&conn.second()).unwrap();
                    let transition_index = self.transitions.get_index_of(&conn.first()).unwrap();
                    adjacency[(place_index, transition_index)] = conn.weight() as f64;
                },
                VertexType::Position => {
                    let place_index = self.positions.get_index_of(&conn.first()).unwrap();
                    let transition_index = self.transitions.get_index_of(&conn.second()).unwrap();
                    adjacency[(place_index, transition_index)] = -(conn.weight() as f64);
                },
            }
        }

        adjacency
    }

    pub fn adjacency_matrices(&self) -> (DMatrix<f64>, DMatrix<f64>) {
        let mut adjacency_input = DMatrix::<f64>::zeros(self.positions.len(), self.positions.len());
        let mut adjacency_output = DMatrix::<f64>::zeros(self.positions.len(), self.positions.len());

        for conn in self.connections.iter() {
            match conn.first().type_ {
                VertexType::Transition => {
                    let place_index = self.positions.get_index_of(&conn.second()).unwrap();
                    let transition_index = self.transitions.get_index_of(&conn.first()).unwrap();
                    adjacency_output[(place_index, transition_index)] = conn.weight() as f64;
                },
                VertexType::Position => {
                    let place_index = self.positions.get_index_of(&conn.first()).unwrap();
                    let transition_index = self.transitions.get_index_of(&conn.second()).unwrap();
                    adjacency_input[(place_index, transition_index)] = -(conn.weight() as f64);
                },
            }
        }

        (adjacency_input, adjacency_output)
    }

    #[inline]
    pub fn update_transition_index(&self) {
        *(*self.transition_index).borrow_mut() += 1;
    }

    #[inline]
    pub fn get_transition_index(&self) -> usize {
        *self.transition_index.borrow_mut()
    }

    pub fn next_transition_index(&self) -> usize {
        let ret = self.get_transition_index();
        self.update_transition_index();
        ret
    }

    pub fn normalize(&mut self) {
        let positions = self.positions.len();
        let transitions = self.transitions.len();

        if positions == 2 * transitions {
            return;
        }

        // NEED = 2 * T - P
        let mut need = 2 * transitions - positions;
        let mut conns = vec![];
        let mut positions = vec![];

        for position in self.positions.values() {
            if need == 0 {
                break;
            }

            let connections = self
                .connections
                .iter()
                .filter(|conn| conn.first().eq(&position.index()) || conn.second().eq(&position.index()))
                .collect::<Vec<_>>();

            if connections.len() < 2 {
                continue;
            }

            let new_pos = position.split(self.next_position_index());
            positions.push(new_pos.clone());

            for conn in connections.into_iter() {
                match conn.first().type_ {
                    VertexType::Transition => conns.push(Connection::new(conn.first().clone(), new_pos.index())),
                    VertexType::Position => conns.push(Connection::new(new_pos.index(), conn.second().clone())),
                }

                // Установим вес соединения
                conns.last_mut().unwrap().set_weight(conn.weight());
            }

            need -= 1;
        }

        positions.into_iter().for_each(|p| {
            self.insert_position(p);
        });
        conns.into_iter().for_each(|c| self.connections.push(c));
    }

    pub fn remove_part(&mut self, part: &Vec<VertexIndex>) -> Self {

        let mut result = PetriNet::new();
        result.position_index = self.position_index.clone();
        result.transition_index = self.transition_index.clone();

        log::info!("PART => {part:?}");

        // Fill result with loop elements
        part.iter().for_each(|element| {
            match element.type_ {
                VertexType::Position => result.insert(self.get_position(element.id).unwrap().clone()),
                VertexType::Transition => result.insert(self.get_transition(element.id).unwrap().clone())
            };
        });

        for connection in part.windows(2) {
            let from = connection[0];
            let to = connection[1];

            result.connect(from, to);
            self.connections.drain_filter(|conn| conn.first().eq(&from) && conn.second().eq(&to));
        }

        if (part.first().unwrap().type_ == VertexType::Position && part.last().unwrap().type_ == VertexType::Transition)
            || (part.first().unwrap().type_ == VertexType::Transition && part.last().unwrap().type_ == VertexType::Position)
        {
            // Это цикл
            result.connect(*part.last().unwrap(), *part.first().unwrap());
            self.connections.drain_filter(|conn| conn.first().eq(part.last().unwrap()) && conn.second().eq(part.first().unwrap()));
        }

        for loop_element in part.iter() {
            let new_element = match loop_element.type_ {
                VertexType::Position => result.get_position(loop_element.id).unwrap().split(self.get_position_index()),
                VertexType::Transition => result.get_transition(loop_element.id).unwrap().split(self.get_transition_index()),
            };

            let mut add_element = false;
            for connection in self.connections.iter_mut() {
                if connection.first().eq(loop_element) {
                    *connection.first_mut() = new_element.index();
                    add_element = true;
                }
                else if connection.second().eq(loop_element) {
                    *connection.second_mut() = new_element.index();
                    add_element = true;
                }
            }

            if add_element {
                match loop_element.type_ {
                    VertexType::Position => {
                        self.insert_position(new_element.clone());
                        self.update_position_index();
                    },
                    VertexType::Transition => {
                        self.insert_transition(new_element.clone());
                        self.update_transition_index();
                    },
                };

                if new_element.is_transition() {
                    // Если переход не имеет выходных соединений
                    if let None = self.connections.iter().find(|c| c.first().eq(&new_element.index())) {
                        let parent = new_element.get_parent().unwrap();
                        if let Some(mut conn) = result
                            .connections
                            .iter()
                            .find(|c| c.first().eq(&parent))
                            .cloned()
                        {
                            let index = self.next_position_index();
                            let pos = self.insert_position(result.get_position(conn.second().id).unwrap().split(index));
                            *conn.first_mut() = new_element.index();
                            *conn.second_mut() = pos.index();
                            self.connections.push(conn);
                        }
                    }

                    if let None = self
                        .connections
                        .iter()
                        .find(|c| c.second().eq(&new_element.index()))
                    {
                        let parent = new_element.get_parent().unwrap();
                        if let Some(mut conn) = result
                            .connections
                            .iter()
                            .find(|c| c.second().type_ == VertexType::Transition && c.second().eq(&parent))
                            .cloned()
                        {
                            let index = self.next_position_index();
                            let pos = self.insert_position(result.get_position(conn.first().id).unwrap().split(index));
                            *conn.first_mut() = pos.index();
                            *conn.second_mut() = new_element.index();
                            self.connections.push(conn);
                        }
                    }
                }
            }

            match loop_element.type_ {
                VertexType::Position => self.positions.remove(loop_element),
                VertexType::Transition => self.transitions.remove(loop_element),
            };
        }

        log::info!("PART RESULT => {result:?}");
        log::info!("SELF => {self:?}");
        result
    }
}

pub fn synthesis_program(programs: &mut DecomposeContext, index: usize) -> PetriNet {

    let positions = programs.positions().len();
    let mut pos_indexes_vec = programs.positions().clone();
    let mut tran_indexes_vec = programs.transitions().clone();

    let c_matrix = programs.c_matrix.clone();
    let (mut adjacency_input, mut adjacency_output) = programs.primitive_matrix.clone();

    let mut markers = programs.marking();

    let (t_sets, p_sets) = programs.programs[index].sets(&pos_indexes_vec, &tran_indexes_vec);

    log::error!("PSET => {:?}", p_sets);
    log::error!("TSET => {:?}", t_sets);

    for t_set in t_sets.into_iter() {
        programs.transition_synthesis_program(&t_set, &mut adjacency_input, &mut adjacency_output);
    }

    for p_set in p_sets.into_iter() {
        programs.position_synthesis_program(&p_set, &mut adjacency_input, &mut adjacency_output, &mut markers);
    }

    adjacency_input = &c_matrix * adjacency_input;
    adjacency_output = &c_matrix * adjacency_output;
    markers = &c_matrix * markers;

    let mut fract = true;
    adjacency_input.iter().for_each(|v| {
        if v.fract() != 0.0 {
            fract = false;
        }
    });

    adjacency_output.iter().for_each(|v| {
        if v.fract() != 0.0 {
            fract = false;
        }
    });

    markers.iter().for_each(|v| {
        if v.fract() != 0.0 {
            fract = false;
        }
    });

    adjacency_input.iter().zip(adjacency_output.iter()).for_each(|(a, b)| {
        if (*a > 0. && *b < 0.) || (*a < 0. && *b > 0.) {
            fract = false;
        }
    });

    if !fract {
        return PetriNet::new();
    }

    let mut remove_rows = vec![];
    for (index, (row_a, row_b)) in adjacency_input.row_iter().zip(adjacency_output.row_iter()).enumerate() {
        if row_a.iter().all(|&e| e == 0.) && row_b.iter().all(|&e| e == 0.) {
            remove_rows.push(index);
            continue;
        }

        for (sub_index, (sub_a, sub_b)) in adjacency_input.row_iter().zip(adjacency_output.row_iter()).enumerate().skip(index + 1) {
            if row_a == sub_a
                && row_b == sub_b
                && markers.row(index) == markers.row(sub_index)
            {
                remove_rows.push(sub_index);
                markers.row_mut(index)[0] = f64::max(
                    markers.row_mut(index)[0],
                    markers.row_mut(sub_index)[0],
                );
            }
        }
    }

    adjacency_input = adjacency_input.remove_rows_at(&remove_rows);
    adjacency_output = adjacency_output.remove_rows_at(&remove_rows);
    markers = markers.remove_rows_at(&remove_rows);
    pos_indexes_vec = pos_indexes_vec
        .into_iter()
        .enumerate()
        .filter(|i| !remove_rows.contains(&i.0))
        .map(|i| i.1.clone())
        .collect();


    let mut remove_cols = vec![];
    for (index, (col_a, col_b)) in adjacency_input.column_iter().zip(adjacency_output.column_iter()).enumerate() {
        if col_a.iter().chain(col_b.iter()).all(|&e| e == 0.) {
            remove_cols.push(index);
            continue;
        }
        for (sub_index, (sub_a, sub_b)) in adjacency_input.column_iter().zip(adjacency_output.column_iter()).enumerate().skip(index + 1) {
            if col_a == sub_a && col_b == sub_b {
                remove_cols.push(sub_index);
            }
        }
    }

    adjacency_input = adjacency_input.remove_columns_at(&remove_cols);
    adjacency_output = adjacency_output.remove_columns_at(&remove_cols);
    tran_indexes_vec = tran_indexes_vec
        .into_iter()
        .enumerate()
        .filter(|i| !remove_cols.contains(&i.0))
        .map(|i| i.1.clone())
        .collect();


    let mut new_net = PetriNet::new();
    new_net.positions = pos_indexes_vec.iter().map(|v| (v.index(), v.clone())).collect();
    new_net.transitions = tran_indexes_vec.iter().map(|v| (v.index(), v.clone())).collect();

    let mut pos_new_indexes = HashMap::new();
    for (index, position) in new_net
        .positions
        .values_mut()
        .enumerate()
    {
        log::info!("SET MARKERS: {} <= {}", index, markers.row(index)[0]);
        position.set_markers(markers.row(index)[0].max(0.) as usize);
        pos_new_indexes.insert(position.index(), index);
    }

    let mut trans_new_indexes = HashMap::new();
    for (index, transition) in new_net
        .transitions
        .values()
        .filter(|e| e.is_transition())
        .enumerate()
    {
        trans_new_indexes.insert(transition.index(), index);
    }

    let mut connections = vec![];
    for transition in new_net.transitions.values() {
        for i in 0..2 {
            let column = match i {
                0 => adjacency_input.column(*trans_new_indexes.get(&transition.index()).unwrap()),
                1 => adjacency_output.column(*trans_new_indexes.get(&transition.index()).unwrap()),
                _ => panic!("Invalid column index"),
            };

            for (index, &el) in column.iter().enumerate().filter(|e| e.1.ne(&0.)) {
                let pos = pos_indexes_vec[index].clone();
                if el < 0. {
                    connections.push(Connection::new(pos.index(), transition.index()));
                } else {
                    connections.push(Connection::new(transition.index(), pos.index()));
                }

                connections.last_mut().unwrap().set_weight(el.abs() as usize);
            }
        }
    }

    new_net.connections = connections;

    new_net
 }


