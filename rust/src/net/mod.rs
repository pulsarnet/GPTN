mod connection;
mod petri_net_vec;
pub mod vertex;

use crate::ffi::matrix::CNamedMatrix;
use core::{NetCycles, NetPaths};
use indexmap::map::IndexMap;
use nalgebra::DMatrix;
pub use net::connection::Connection;
pub use net::vertex::Vertex;
use net::vertex::{VertexIndex, VertexType};
use std::collections::HashMap;
use std::sync::atomic::{AtomicUsize, Ordering};
use tracing::debug;

pub use net::petri_net_vec::*;

#[derive(Debug)]
pub struct PetriNet {
    /// Позиции сети Петри
    positions: IndexMap<VertexIndex, Vertex>,

    /// Переходы сети Петри
    transitions: IndexMap<VertexIndex, Vertex>,

    /// Соединения между вершинами в сети Петри
    connections: Vec<Connection>,

    /// Последняя добавленная позиция
    position_index: AtomicUsize,

    /// Последний добавленный переход
    transition_index: AtomicUsize,
}

impl PetriNet {
    pub fn new() -> Self {
        PetriNet::default()
    }

    pub fn positions(&self) -> &IndexMap<VertexIndex, Vertex> {
        &self.positions
    }

    pub fn positions_mut(&mut self) -> &mut IndexMap<VertexIndex, Vertex> {
        &mut self.positions
    }

    pub fn transitions(&self) -> &IndexMap<VertexIndex, Vertex> {
        &self.transitions
    }

    pub fn transitions_mut(&mut self) -> &mut IndexMap<VertexIndex, Vertex> {
        &mut self.transitions
    }

    pub fn connections(&self) -> &[Connection] {
        &self.connections
    }

    pub fn connections_mut(&mut self) -> &mut Vec<Connection> {
        &mut self.connections
    }

    /// Количество выходных позиций (т.е. позиций без выходных дуг)
    ///
    /// TODO: сделать итератор (output_positions_iter, output_positions - коллекция)
    pub fn output_positions(&self) -> usize {
        let mut post_c = 0;
        for pos in self.positions.keys() {
            let mut filter = self.connections.iter().filter(|conn| conn.first() == *pos);

            if filter.next().is_none() {
                post_c += 1;
            }
        }
        post_c
    }

    /// Количество входных позиций (т.е. позиций без входных дуг)
    ///
    /// TODO: сделать итератор (input_positions_iter, input_positions - коллекция)
    pub fn input_positions(&self) -> usize {
        let mut pre_c = 0;
        for pos in self.positions.keys() {
            let mut filter = self.connections.iter().filter(|conn| conn.second() == *pos);

            if filter.next().is_none() {
                pre_c += 1;
            }
        }
        pre_c
    }

    /// Массив индексов всех вершин
    ///
    /// TODO: хранить отдельно в структуре сети, сделать возврат Slice.
    pub fn vertices(&self) -> Vec<VertexIndex> {
        self.positions
            .iter()
            .chain(self.transitions.iter())
            .map(|(index, _)| *index)
            .collect::<Vec<_>>()
    }

    /// Получить позицию по индексу [`VertexIndex`]
    pub fn get_position(&self, index: usize) -> Option<&Vertex> {
        self.positions.get(&VertexIndex::position(index))
    }

    /// Удалить позицию по индексу [`VertexIndex`]
    pub fn remove_position(&mut self, index: usize) {
        let position = self.get_position(index).unwrap().index();
        self.connections
            .retain(|c| c.first().ne(&position) && c.second().ne(&position));
            //.drain_filter(|c| c.first().eq(&position) || c.second().eq(&position));
        self.positions.remove(&position);
    }

    /// Получить переход по индексу [`VertexIndex`]
    pub fn get_transition(&self, index: usize) -> Option<&Vertex> {
        self.transitions.get(&VertexIndex::transition(index))
    }

    /// Удалить переход по индексу [`VertexIndex`]
    pub fn remove_transition(&mut self, index: usize) {
        let transition = self.get_transition(index).unwrap().index();
        self.connections
            .retain(|c| c.first().ne(&transition) && c.second().ne(&transition));
            //.drain_filter(|c| c.first().eq(&transition) || c.second().eq(&transition));
        self.transitions.remove(&transition);
    }

    /// Добавить позицию по индексу [`VertexIndex`]
    pub fn add_position(&mut self, index: usize) -> &Vertex {
        debug!("add new position with {index}");
        // Если текущее значение меньше, тогда установить новое
        self.position_index.fetch_max(index + 1, Ordering::SeqCst);
        self.positions
            .entry(VertexIndex::position(index))
            .or_insert_with(|| Vertex::position(index))
    }

    /// Добавить переход по индексу [`VertexIndex`]
    pub fn add_transition(&mut self, index: usize) -> &Vertex {
        debug!("add new transition with {index}");
        // Если текущее значение меньше, тогда установить новое
        self.transition_index.fetch_max(index + 1, Ordering::SeqCst);
        self.transitions
            .entry(VertexIndex::transition(index))
            .or_insert_with(|| Vertex::transition(index))
    }

    /// Вставить позицию
    ///
    /// Добавляет переданную позицию [`Vertex`] в сеть Петри [`PetriNet`]
    ///
    /// # Return
    /// Ссылку на добавленный или существующий [`Vertex`]
    pub fn insert_position(&mut self, element: Vertex) -> &Vertex {
        match self.positions.contains_key(&element.index()) {
            true => self.positions.get(&element.index()).unwrap(),
            false => {
                let index = element.index();
                // Если текущее значение меньше, тогда установить новое
                self.position_index.fetch_max(index.id + 1, Ordering::SeqCst);

                if let Some(parent_index) = element.get_parent() {
                    // Найдем индекс родителя и вставим позицию после
                    match self
                        .positions
                        .iter()
                        .position(|(pos_index, _)| *pos_index == parent_index)
                    {
                        Some(i) => {
                            let new_map = self.positions.split_off(i + 1);
                            self.positions.insert(index, element);
                            self.positions.extend(new_map.into_iter());
                        }
                        None => {
                            self.positions.insert(index, element);
                        }
                    }
                } else {
                    self.positions.insert(index, element);
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
                // Если текущее значение меньше, тогда установить новое
                self.transition_index.fetch_max(index.id + 1, Ordering::SeqCst);

                if let Some(parent_index) = element.get_parent() {
                    // Найдем индекс родителя и вставим переход после
                    match self
                        .transitions
                        .iter()
                        .position(|(tran_index, _)| *tran_index == parent_index)
                    {
                        Some(i) => {
                            let new_map = self.transitions.split_off(i + 1);
                            self.transitions.insert(index, element);
                            self.transitions.extend(new_map.into_iter());
                        }
                        None => {
                            self.transitions.insert(index, element);
                        }
                    }
                } else {
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

    /// Соединить две вершины
    ///
    /// # Panic
    /// Метод паникует, если вершины одного типа
    pub fn connect(&mut self, a: VertexIndex, b: VertexIndex) {
        if a.type_ == b.type_ {
            panic!("Vertex {} has same type as {}", a, b);
        }

        match self
            .connections
            .iter_mut()
            .find(|c| c.first() == a && c.second() == b)
        {
            Some(connection) => connection.increment(),
            None => self.connections.push(Connection::new(a, b)),
        }
    }

    pub fn get_loop(&self) -> Option<Vec<VertexIndex>> {
        let cycles = NetCycles::find(self);
        cycles.get_longest().map(Vec::from)
    }

    pub fn get_part(&self) -> Option<Vec<VertexIndex>> {
        let paths = NetPaths::find(self);
        paths.get_longest().map(Vec::from)
    }

    #[inline]
    #[must_use]
    pub fn update_position_index(&self) -> usize {
        self.position_index.fetch_add(1, Ordering::SeqCst)
    }

    #[inline]
    pub fn get_position_index(&self) -> usize {
        self.position_index.load(Ordering::SeqCst)
    }

    pub fn next_position_index(&self) -> usize {
        self.update_position_index()
    }

    pub fn as_matrix(&self) -> (CNamedMatrix, CNamedMatrix) {
        let positions = self.positions.clone();
        let transitions = self.transitions.clone();

        let pos_indexes = positions
            .values()
            .enumerate()
            .map(|(i, v)| (v.index(), i))
            .collect::<HashMap<_, _>>();
        let tran_indexes = transitions
            .values()
            .enumerate()
            .map(|(i, v)| (v.index(), i))
            .collect::<HashMap<_, _>>();

        let mut d_input = DMatrix::<i32>::zeros(positions.len(), transitions.len());
        let mut d_output = DMatrix::<i32>::zeros(positions.len(), transitions.len());

        for conn in self.connections.iter() {
            match conn.first().type_ {
                VertexType::Position => {
                    d_input.row_mut(*pos_indexes.get(&conn.first()).unwrap())
                        [*tran_indexes.get(&conn.second()).unwrap()] = -(conn.weight() as i32)
                }
                VertexType::Transition => {
                    d_output.column_mut(*tran_indexes.get(&conn.first()).unwrap())
                        [*pos_indexes.get(&conn.second()).unwrap()] = conn.weight() as i32
                }
            }
        }

        (
            CNamedMatrix::new(&positions, &transitions, d_input),
            CNamedMatrix::new(&positions, &transitions, d_output),
        )
    }

    pub fn incidence_matrix(&self) -> (CNamedMatrix, CNamedMatrix) {
        let positions = self.positions.values().collect::<Vec<_>>();
        let transitions = self.transitions.values().collect::<Vec<_>>();

        let position_indexes = positions
            .iter()
            .enumerate()
            .map(|(i, v)| (v.index(), i))
            .collect::<HashMap<_, _>>();
        let transition_indexes = transitions
            .iter()
            .enumerate()
            .map(|(i, v)| (v.index(), i))
            .collect::<HashMap<_, _>>();

        let mut input = DMatrix::<i32>::zeros(positions.len(), transitions.len());
        let mut output = DMatrix::<i32>::zeros(positions.len(), transitions.len());

        for conn in self.connections.iter() {
            match conn.first().type_ {
                VertexType::Transition => {
                    output.row_mut(*position_indexes.get(&conn.second()).unwrap())
                        [*transition_indexes.get(&conn.first()).unwrap()] = conn.weight() as i32
                }
                VertexType::Position => {
                    input.row_mut(*position_indexes.get(&conn.first()).unwrap())
                        [*transition_indexes.get(&conn.second()).unwrap()] = conn.weight() as i32
                }
            }
        }

        (
            CNamedMatrix::new(&self.positions, &self.transitions, input),
            CNamedMatrix::new(&self.positions, &self.transitions, output),
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
                }
                VertexType::Position => {
                    let place_index = self.positions.get_index_of(&conn.first()).unwrap();
                    let transition_index = self.transitions.get_index_of(&conn.second()).unwrap();
                    adjacency[(place_index, transition_index)] = -(conn.weight() as f64);
                }
            }
        }

        adjacency
    }

    pub fn adjacency_matrices(&self) -> (DMatrix<f64>, DMatrix<f64>) {
        let mut adjacency_input =
            DMatrix::<f64>::zeros(self.positions.len(), self.transitions.len());
        let mut adjacency_output =
            DMatrix::<f64>::zeros(self.positions.len(), self.transitions.len());

        for conn in self.connections.iter() {
            match conn.first().type_ {
                VertexType::Transition => {
                    let place_index = self.positions.get_index_of(&conn.second()).unwrap();
                    let transition_index = self.transitions.get_index_of(&conn.first()).unwrap();
                    adjacency_output[(place_index, transition_index)] = conn.weight() as f64;
                }
                VertexType::Position => {
                    let place_index = self.positions.get_index_of(&conn.first()).unwrap();
                    let transition_index = self.transitions.get_index_of(&conn.second()).unwrap();
                    adjacency_input[(place_index, transition_index)] = -(conn.weight() as f64);
                }
            }
        }

        (adjacency_input, adjacency_output)
    }

    #[inline]
    #[must_use]
    pub fn update_transition_index(&self) -> usize {
        self.transition_index.fetch_add(1, Ordering::SeqCst)
    }

    #[inline]
    pub fn get_transition_index(&self) -> usize {
        self.transition_index.load(Ordering::SeqCst)
    }

    pub fn next_transition_index(&self) -> usize {
        self.update_transition_index()
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
                .filter(|conn| {
                    conn.first().eq(&position.index()) || conn.second().eq(&position.index())
                })
                .collect::<Vec<_>>();

            if connections.len() < 2 {
                continue;
            }

            let new_pos = position.split(self.next_position_index());
            positions.push(new_pos.clone());

            for conn in connections.into_iter() {
                match conn.first().type_ {
                    VertexType::Transition => {
                        conns.push(Connection::new(conn.first().clone(), new_pos.index()))
                    }
                    VertexType::Position => {
                        conns.push(Connection::new(new_pos.index(), conn.second().clone()))
                    }
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
        result.position_index = AtomicUsize::new(self.position_index.load(Ordering::SeqCst));
        result.transition_index = AtomicUsize::new(self.transition_index.load(Ordering::SeqCst));

        debug!("PART => {part:?}");

        // Fill result with loop elements
        part.iter().for_each(|element| {
            match element.type_ {
                VertexType::Position => {
                    result.insert(self.get_position(element.id).unwrap().clone())
                }
                VertexType::Transition => {
                    result.insert(self.get_transition(element.id).unwrap().clone())
                }
            };
        });

        for connection in part.windows(2) {
            let from = connection[0];
            let to = connection[1];

            result.connect(from, to);
            self.connections
                .retain(|conn| conn.first().ne(&from) || conn.second().ne(&to));
                //.drain_filter(|conn| conn.first().eq(&from) && conn.second().eq(&to));
        }

        // todo заменить на сравнение равенства типов first и last
        if (part.first().unwrap().type_ == VertexType::Position
            && part.last().unwrap().type_ == VertexType::Transition)
            || (part.first().unwrap().type_ == VertexType::Transition
                && part.last().unwrap().type_ == VertexType::Position)
        {
            // Это цикл
            result.connect(*part.last().unwrap(), *part.first().unwrap());
            self.connections
                .retain(|conn| conn.first().ne(part.last().unwrap()) || conn.second().ne(part.first().unwrap()));
                //.drain_filter(|conn| { conn.first().eq(part.last().unwrap()) && conn.second().eq(part.first().unwrap()) });
        }

        for loop_element in part.iter() {
            let new_element = match loop_element.type_ {
                VertexType::Position => result
                    .get_position(loop_element.id)
                    .unwrap()
                    .split(self.get_position_index()),
                VertexType::Transition => result
                    .get_transition(loop_element.id)
                    .unwrap()
                    .split(self.get_transition_index()),
            };

            let mut add_element = false;
            for connection in self.connections.iter_mut() {
                if connection.first().eq(loop_element) {
                    *connection.first_mut() = new_element.index();
                    add_element = true;
                } else if connection.second().eq(loop_element) {
                    *connection.second_mut() = new_element.index();
                    add_element = true;
                }
            }

            if add_element {
                match loop_element.type_ {
                    VertexType::Position => {
                        self.insert_position(new_element.clone());
                        let next = self.update_position_index();
                        debug!("next position index: {}", next);
                    }
                    VertexType::Transition => {
                        self.insert_transition(new_element.clone());
                        let next = self.update_transition_index();
                        debug!("next transition index: {}", next);
                    }
                };

                if new_element.is_transition() {
                    // Если переход не имеет выходных соединений
                    if let None = self
                        .connections
                        .iter()
                        .find(|c| c.first().eq(&new_element.index()))
                    {
                        let parent = new_element.get_parent().unwrap();
                        if let Some(mut conn) = result
                            .connections
                            .iter()
                            .find(|c| c.first().eq(&parent))
                            .cloned()
                        {
                            let index = self.next_position_index();
                            let pos = self.insert_position(
                                result.get_position(conn.second().id).unwrap().split(index),
                            );
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
                            .find(|c| {
                                c.second().type_ == VertexType::Transition && c.second().eq(&parent)
                            })
                            .cloned()
                        {
                            let index = self.next_position_index();
                            let pos = self.insert_position(
                                result.get_position(conn.first().id).unwrap().split(index),
                            );
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

        debug!("PART RESULT => {result:?}");
        debug!("SELF => {self:?}");
        result
    }

    pub fn marking(&self) -> DMatrix<i32> {
        self.positions.values().enumerate().fold(
            DMatrix::zeros(1, self.positions.len()),
            |mut acc, (i, vert)| {
                acc.row_mut(0)[i] = vert.markers() as i32;
                acc
            },
        )
    }

    pub fn adjacent(&self, vertex: VertexIndex) -> Vec<VertexIndex> {
        self.connections
            .iter()
            .filter(|conn| conn.first() == vertex)
            .map(|conn| conn.second())
            .collect()
    }
}

impl Default for PetriNet {
    fn default() -> Self {
        PetriNet {
            positions: IndexMap::new(),
            transitions: IndexMap::new(),
            connections: vec![],
            position_index: AtomicUsize::new(1),
            transition_index: AtomicUsize::new(1),
        }
    }
}

impl Clone for PetriNet {
    fn clone(&self) -> Self {
        Self {
            positions: self.positions.clone(),
            transitions: self.transitions.clone(),
            connections: self.connections.clone(),
            position_index: AtomicUsize::new(self.position_index.load(Ordering::SeqCst)),
            transition_index: AtomicUsize::new(self.transition_index.load(Ordering::SeqCst)),
        }
    }
}

#[cfg(test)]
mod tests {


}