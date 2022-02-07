mod connection;
mod vertex;

use core::MatrixFormat;
use nalgebra::DMatrix;
use ndarray_linalg::Solve;
pub use net::connection::Connection;
pub use net::vertex::Vertex;
use std::cell::RefCell;
use std::cmp::max;
use std::collections::{HashMap, HashSet};
use std::rc::Rc;
use libc::abs;
use std::fmt::Write;
use log::{info, log};
use {SynthesisContext};
use ::{CMatrix, NamedMatrix};
use SynthesisProgram;

#[derive(Debug)]
pub struct PetriNet {
    pub positions: Vec<Vertex>,
    pub transitions: Vec<Vertex>,
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
            .flat_map(|n| n.positions.iter())
            .cloned()
            .collect()
    }

    pub fn transitions(&self) -> Vec<Vertex> {
        self.0
            .iter()
            .flat_map(|n| n.transitions.iter())
            .cloned()
            .collect()
    }

    pub fn sort(&mut self) {
        self.0.sort_by(|net_a, net_b| {
            net_b.positions.len().cmp(&net_a.positions.len())
        });
    }

    pub fn position_indexes(&self) -> HashMap<Vertex, usize> {
        self.0
            .iter()
            .flat_map(|net| net.positions.iter())
            .enumerate()
            .map(|(i, v)| (v.clone(), i))
            .collect()
    }

    pub fn transition_indexes(&self) -> HashMap<Vertex, usize> {
        self.0
            .iter()
            .flat_map(|net| net.transitions.iter())
            .enumerate()
            .map(|(i, v)| (v.clone(), i))
            .collect()
    }

    pub fn primitive_net(&self) -> PetriNet {
        let mut result = PetriNet::new();

        for net in self.0.iter() {
            let transitions = net.transitions.clone();

            'brk: for transition in transitions.into_iter() {
                let mut vertexes = vec![
                    Vertex::position(0),
                    Vertex::transition(0),
                    Vertex::position(0),
                ];
                let mut from = net.connections.iter().filter(|c| c.first().eq(&transition));

                while let Some(f) = from.next() {
                    if result.positions.iter().find(|v: &&Vertex| (*v).eq(f.second())).is_some() {
                        continue;
                    }

                    let mut to = net
                        .connections
                        .iter()
                        .filter(|c| c.first().ne(f.second()) && c.second().eq(&transition));

                    while let Some(t) = to.next() {
                        if result.positions.iter().find(|v: &&Vertex| (*v).eq(t.first())).is_some() {
                            continue;
                        }

                        result.insert(t.first().clone());
                        result.insert(f.first().clone());
                        result.insert(f.second().clone());

                        result.connect(t.first().clone(), f.first().clone());
                        result.connect(f.first().clone(), f.second().clone());
                        continue 'brk;
                    }
                }
            }
        }

        result
    }


    pub fn primitive(&self) -> (PetriNet, DMatrix<i32>) {
        let positions = self.position_indexes();
        let transitions = self.transition_indexes();
        let mut result = DMatrix::<i32>::zeros(positions.len(), transitions.len());

        let primitive_net = self.primitive_net();
        for connection in primitive_net.connections.iter() {
            if connection.first().is_position() {
                result.row_mut(*positions.get(connection.first()).unwrap())[*transitions.get(connection.second()).unwrap()] = -1;
            }
            else {
                result.column_mut(*transitions.get(connection.first()).unwrap())[*positions.get(connection.second()).unwrap()] = 1;
            }
        }

        (primitive_net, result)
    }
    
    pub fn elements(&self) -> Vec<Vertex> {
        
        self.0
            .iter()
            .flat_map(|n| n.positions.iter().chain(n.transitions.iter()))
            .cloned()
            .collect()
        
    }

    pub fn connections(&self) -> Vec<&Connection> {

        self.0
            .iter()
            .flat_map(|n| n.connections.iter())
            .collect()

    }

    pub fn equivalent_matrix(&self) -> (CMatrix, CMatrix) {
        
        let pos_indexes = self.position_indexes();
        let tran_indexes = self.transition_indexes();
        
        let all_elements = self.elements();
        let all_connections = self.connections();

        let mut d_input = nalgebra::DMatrix::<i32>::zeros(pos_indexes.len(), tran_indexes.len());
        let mut d_output = nalgebra::DMatrix::<i32>::zeros(pos_indexes.len(), tran_indexes.len());

        for connection in all_connections {
            let first = connection.first();
            let second = connection.second();

            if first.is_transition() {
                d_output.column_mut(*tran_indexes.get(first).unwrap())[*pos_indexes.get(second).unwrap()] = 1;
            }
            else {
                d_input.row_mut(*pos_indexes.get(first).unwrap())[*tran_indexes.get(second).unwrap()] = -1;
            }
        }

        (CMatrix::from(d_input), CMatrix::from(d_output))
    }

    pub fn lbf_matrix(&self) -> Vec<(NamedMatrix, NamedMatrix)> {
        self.0.iter().map(|net| net.as_matrix()).collect()
    }

}

impl Clone for PetriNet {
    fn clone(&self) -> Self {
        let new_connections = self
            .connections
            .iter()
            .map(|conn| Connection::new(conn.first().clone(), conn.second().clone()))
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
            positions: vec![],
            transitions: vec![],
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
        self.positions
            .iter()
            .find(|p| (*p).index().eq(&index))
    }

    pub fn remove_position(&mut self, index: u64) {
        let position = self.get_position(index).unwrap().index();
        self.connections
            .drain_filter(|c| c.first().is_position() && c.first().index().eq(&position) || c.second().is_position() && c.first().index().eq(&position));
        self.positions
            .remove(self.positions.iter().position(|e| e.index().eq(&position)).unwrap());
    }

    pub fn get_transition(&self, index: u64) -> Option<&Vertex> {
        self.transitions
            .iter()
            .find(|p| (*p).index().eq(&index))
    }

    pub fn remove_transition(&mut self, index: u64) {
        let transition = self.get_transition(index).unwrap().index();
        self.connections
            .drain_filter(|c| c.first().is_transition() && c.first().index().eq(&transition) || c.second().is_transition() && c.second().index().eq(&transition));
        self.transitions.remove(
            self.transitions
                .iter()
                .position(|e| e.index().eq(&transition))
                .unwrap(),
        );
    }

    pub fn add_position(&mut self, index: u64) -> &Vertex {
        if let Some(i) = (0..self.positions.len()).find(|&i| self.positions[i].index() == index) {
            &self.positions[i]
        }
        else {
            self.positions.push(Vertex::position(index));
            if index >= self.get_position_index() {
                *(*self.position_index).borrow_mut() = index + 1;
            }
            self.positions.last().unwrap()
        }
    }

    pub fn add_transition(&mut self, index: u64) -> &Vertex {
        if let Some(i) = (0..self.transitions.len()).find(|&i| self.transitions[i].index() == index) {
            &self.transitions[i]
        }
        else {
            self.transitions.push(Vertex::transition(index));
            if index >= self.get_transition_index() {
                *(*self.transition_index).borrow_mut() = index + 1;
            }
            self.transitions.last().unwrap()
        }
    }

    pub fn insert_position(&mut self, element: Vertex, insert: bool) -> &Vertex {
        if let Some(p) = self
            .positions
            .iter()
            .enumerate()
            .find(|(index, el)| el.index() == element.index())
            .map(|(index, _)| index)
        {
            log::info!("Element already exists {:?}", p);
            return &self.positions[p];
        }

        if element.index() >= self.get_position_index() {
            *(*self.position_index).borrow_mut() = element.index() + 1;
        }

        let found = self.positions
            .iter()
            .enumerate()
            .find(|e| element.get_parent().contains(&e.1.index()))
            .map(|e| e.0);

        if let (Some(found), true) = (found, insert) {
            self.positions.insert(found + 1, element);
        } else {
            self.positions.push(element);
        }

        self.positions.last().unwrap()
    }

    pub fn insert_transition(&mut self, element: Vertex, insert: bool) -> &Vertex {
        if let Some(p) = self
            .transitions
            .iter()
            .enumerate()
            .find(|(index, el)| el.index() == element.index())
            .map(|(index, _)| index)
        {
            log::info!("Element already exists {:?}", p);
            return &self.transitions[p];
        }

        if element.index() >= self.get_transition_index() {
            *(*self.transition_index).borrow_mut() = element.index() + 1;
        }

        if let (Some(found), true) = (
            self.transitions
                .iter()
                .enumerate()
                .find(|e| element.get_parent().contains(&e.1.index()))
                .map(|e| e.0),
            insert, )
        {
            self.transitions.insert(found + 1, element);
        } else {
            self.transitions.push(element);
        }

        self.transitions.last().unwrap()
    }

    pub fn insert(&mut self, element: Vertex) -> &Vertex {
        match element.is_position() {
            true => self.insert_position(element, false),
            false => self.insert_transition(element, false),
        }
    }

    pub fn connect(&mut self, a: Vertex, b: Vertex) {
        self.connections.push(Connection::new(a, b));
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

    pub fn get_part(&self) -> Option<Vec<Vertex>> {
        let mut result = Vec::new();

        for vertex in self.positions.iter() {
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

        let positions = self.positions.clone();
        let transitions = self.transitions.clone();

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
        let positions = self.positions.len();
        let transitions = self.transitions.len();

        if positions == 2 * transitions {
            return;
        }

        // NEED = 2 * T - P
        let mut need = 2 * transitions - positions;
        let mut conns = vec![];
        let mut positions = vec![];

        for position in self.positions.iter() {
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
                        self.insert_transition(new_element.clone(), true);
                        self.update_transition_index();
                    },
                };

                if new_element.is_transition() {
                    if let None = self.connections.iter().find(|c| c.first().eq(&new_element)) {
                        let parent = new_element.get_parent().unwrap();
                        if let Some(mut conn) = result
                            .connections
                            .iter()
                            .find(|c| c.first().is_transition() && c.first().index().eq(&parent))
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
                            .find(|c| c.second().is_transition() && c.second().index().eq(&parent))
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

            if loop_element.is_position() {
                self.positions.remove(self.positions.iter().position(|e| e.index().eq(&loop_element.index())).unwrap());
            }
            else {
                self.transitions.remove(self.transitions.iter().position(|e| e.index().eq(&loop_element.index())).unwrap());
            }


        }

        log::info!("PART RESULT => {result:?}");
        log::info!("SELF => {self:?}");
        result
    }
}

pub fn synthesis_program(programs: &mut SynthesisContext, index: usize) {

    let positions = programs.positions().len();
    let transitions = programs.transitions().len();
    let mut pos_indexes_vec = programs.positions().clone();
    let mut tran_indexes_vec = programs.transitions().clone();
    let c_matrix = programs.c_matrix.inner.clone();
    let lbf_matrix = programs.decompose_context.primitive_matrix.inner.clone();
    let mut markers = nalgebra::DMatrix::<i32>::zeros(positions, 1);
    programs
        .positions()
        .iter()
        .map(Vertex::markers)
        .enumerate()
        .for_each(|e| markers.row_mut(e.0)[0] = e.1 as i32);

    let mut result = nalgebra::DMatrix::<i32>::zeros(positions, transitions);
    let mut save_vec = nalgebra::DMatrix::<i32>::zeros(positions, transitions);
    let current_program = &programs.programs[index].data;
    let mut t_sets = vec![];
    let mut p_sets = vec![];
    let mut searched = vec![0].into_iter().collect::<HashSet<usize>>();

    for index_a in 0..tran_indexes_vec.len() {
        let search_number = current_program[index_a];
        if searched.contains(&search_number) {
            continue;
        }
        let mut indexes = vec![];
        for index_b in (0..tran_indexes_vec.len()).filter(|e| current_program[*e] == search_number)
        {
            if index_a == index_b {
                continue;
            }
            indexes.push(index_b);
        }
        if indexes.len() > 0 {
            indexes.push(index_a);
            t_sets.push(indexes);
        }
        searched.insert(search_number);
    }

    let offset = tran_indexes_vec.len();
    let mut searched = vec![0].into_iter().collect::<HashSet<usize>>();
    for index_a in offset..(offset + pos_indexes_vec.len()) {
        let search_number = current_program[index_a];
        if searched.contains(&search_number) {
            continue;
        }
        let mut indexes = vec![];
        for index_b in (offset..(offset + pos_indexes_vec.len()))
            .filter(|e| current_program[*e] == search_number)
        {
            if index_a == index_b {
                continue;
            }
            indexes.push(index_b - offset);
        }
        if indexes.len() > 0 {
            indexes.push(index_a - offset);
            p_sets.push(indexes);
        }
        searched.insert(search_number);
    }

    log::info!("PSET => {:?}", p_sets);
    log::info!("TSET => {:?}", t_sets);
    log::info!("PRIMITIVE => {}", MatrixFormat(&lbf_matrix, &tran_indexes_vec, &pos_indexes_vec));

    let mut d_input = nalgebra::DMatrix::<i32>::zeros(positions, transitions);
    let mut d_output = nalgebra::DMatrix::<i32>::zeros(positions, transitions);
    for i in 0..d_input.nrows() {
        for j in 0..d_input.ncols() {
            match lbf_matrix.row(i)[j] {
                v if v < 0 => d_input.row_mut(i)[j] = v,
                v if v > 0 => d_output.row_mut(i)[j] = v,
                _ => {}
            }
        }
    }
    log::info!(
         "D INPUT START => {}",
         MatrixFormat(&d_input, &tran_indexes_vec, &pos_indexes_vec)
     );
    log::info!(
         "D OUTPUT START => {}",
         MatrixFormat(&d_output, &tran_indexes_vec, &pos_indexes_vec)
     );

    for t_set in t_sets.into_iter() {
        programs.transition_synthesis_program(&t_set, &mut d_input, &mut d_output);
        log::info!(
             "D INPUT AFTER T => {:?}{}",
             t_set,
             MatrixFormat(&d_input, &tran_indexes_vec, &pos_indexes_vec)
         );
    }

    for p_set in p_sets.into_iter() {
        programs.position_synthesis_program(&p_set, &mut d_input, &mut d_output);
        log::info!(
             "D INPUT AFTER P => {:?}{}",
             p_set,
             MatrixFormat(&d_input, &tran_indexes_vec, &pos_indexes_vec)
         );
    }

    log::info!(
         "D_INPUT => {}",
         MatrixFormat(&d_input, &tran_indexes_vec, &pos_indexes_vec)
     );
    log::info!(
         "D_OUTPUT => {}",
         MatrixFormat(&d_output, &tran_indexes_vec, &pos_indexes_vec)
     );

    let mut d_matrix = nalgebra::DMatrix::<i32>::zeros(positions, transitions);
    for i in 0..d_matrix.nrows() {
        for j in 0..d_matrix.ncols() {
            if (d_input.row(i)[j] + d_output.row(i)[j]) == 0 && d_input.row(i)[j] != 0 {
                d_matrix.row_mut(i)[j] = 0;
                save_vec.row_mut(i)[j] = 1;
            } else {
                d_matrix.row_mut(i)[j] = d_input.row(i)[j] + d_output.row(i)[j];
            }
        }
    }

    log::info!(
         "D_MATRIX BEFORE => {}",
         MatrixFormat(&d_matrix, &tran_indexes_vec, &pos_indexes_vec)
     );

    d_matrix = c_matrix.clone() * d_matrix;
    markers = c_matrix.clone() * markers;

    log::info!(
         "D_MATRIX CMAT => {}",
         MatrixFormat(&d_matrix, &tran_indexes_vec, &pos_indexes_vec)
     );

    //save_vec = c_matrix.clone() * save_vec;

    for i in 0..d_matrix.nrows() {
        for j in 0..d_matrix.ncols() {
            if save_vec.row(i)[j] == 1 {
                d_matrix.row_mut(i)[j] = 0;
            }
        }
    }

    log::info!(
         "D MATRIX => {}",
         MatrixFormat(&d_matrix, &tran_indexes_vec, &pos_indexes_vec)
     );

    log::info!(
         "SAVE => {}",
         MatrixFormat(&save_vec, &tran_indexes_vec, &pos_indexes_vec)
     );


    let mut remove_rows = vec![];
    for (index_a, row_a) in d_matrix.row_iter().enumerate() {
        if row_a.iter().all(|e| *e == 0)
            && save_vec.row(index_a).iter().all(|&e| e == 0)
        {
            remove_rows.push(index_a);
            continue;
        }
        for (index_b, row_b) in d_matrix.row_iter().enumerate().skip(index_a) {
            if index_a == index_b {
                continue;
            }
            if row_a == row_b && save_vec.row(index_a) == save_vec.row(index_b) {
                remove_rows.push(index_b);
                // При объединении эквивалентных позиций выбирается максимальное количество маркеров
                markers.row_mut(index_a)[0] = max(
                    pos_indexes_vec[index_a].markers(),
                    pos_indexes_vec[index_b].markers(),
                ) as i32;
            }
        }
    }

    d_matrix = d_matrix.remove_rows_at(&remove_rows);
    save_vec = save_vec.remove_rows_at(&remove_rows);
    markers = markers.remove_rows_at(&remove_rows);
    pos_indexes_vec = pos_indexes_vec
        .into_iter()
        .enumerate()
        .filter(|i| !remove_rows.contains(&i.0))
        .map(|i| i.1.clone())
        .collect();


    let mut remove_cols = vec![];
    for (index_a, column_a) in d_matrix.column_iter().enumerate() {
        if column_a.iter().all(|e| *e == 0)
            && save_vec.column(index_a).iter().all(|&e| e == 0)
        {
            remove_cols.push(index_a);
            continue;
        }
        for (index_b, column_b) in d_matrix.column_iter().enumerate().skip(index_a) {
            if index_a == index_b {
                continue;
            }
            if column_a == column_b && save_vec.column(index_a) == save_vec.column(index_b) {
                remove_cols.push(index_b);
            }
        }
    }
    d_matrix = d_matrix.remove_columns_at(&remove_cols);
    save_vec = save_vec.remove_columns_at(&remove_cols);
    tran_indexes_vec = tran_indexes_vec
        .into_iter()
        .enumerate()
        .filter(|i| !remove_cols.contains(&i.0))
        .map(|i| i.1.clone())
        .collect();

    log::info!(
         "SAVE 2 => {}",
         MatrixFormat(&save_vec, &tran_indexes_vec, &pos_indexes_vec)
     );
    log::info!(
         "RESULT 2 => {}",
         MatrixFormat(&d_matrix, &tran_indexes_vec, &pos_indexes_vec)
     );


    let mut new_net = PetriNet::new();
    new_net.positions = pos_indexes_vec.iter().cloned().collect();
    new_net.transitions = tran_indexes_vec.iter().cloned().collect();

    let mut pos_new_indexes = HashMap::new();
    for (index, position) in new_net
        .positions
        .iter_mut()
        .enumerate()
    {
        log::info!("SET MARKERS: {} <= {}", index, markers.row(index)[0]);
        position.set_markers(markers.row(index)[0] as u64);
        pos_new_indexes.insert(position.clone(), index);
    }

    let mut trans_new_indexes = HashMap::new();
    for (index, transition) in new_net
        .transitions
        .iter()
        .filter(|e| e.is_transition())
        .enumerate()
    {
        trans_new_indexes.insert(transition.clone(), index);
    }

    let mut connections = vec![];
    for transition in new_net.transitions.iter() {
        let col = d_matrix.column(*trans_new_indexes.get(transition).unwrap());
        for (index, el) in col.iter().enumerate().filter(|e| e.1.ne(&0)) {
            let pos = pos_indexes_vec[index].clone();
            match *el > 0 {
                false => (0..el.abs()).into_iter().for_each(|_| connections.push(Connection::new(pos.clone(), transition.clone()))),
                true => (0..el.abs()).into_iter().for_each(|_| connections.push(Connection::new(transition.clone(), pos.clone()))),
            }
        }
    }
    for i in 0..save_vec.nrows() {
        for j in 0..save_vec.ncols() {
            if save_vec.row(i)[j] == 0 {
                continue;
            }
            let pos = pos_indexes_vec[i].clone();
            let tran = tran_indexes_vec[j].clone();
            connections.push(Connection::new(tran.clone(), pos.clone()));
            connections.push(Connection::new(pos, tran));
        }
    }
    new_net.connections = connections;

    programs.programs[index].net_after = Some(new_net);

 }


