mod connection;
mod petri_net_vec;
pub mod vertex;

use crate::ffi::matrix::CNamedMatrix;
use indexmap::map::IndexMap;
use nalgebra::DMatrix;
pub use net::connection::Connection;
pub use net::vertex::Vertex;
use net::vertex::{VertexIndex, VertexType};
use std::collections::HashMap;
use std::sync::atomic::{AtomicUsize, Ordering};
use tracing::{debug, trace};

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

    /// Получить элемент по индексу [`VertexIndex`]
    pub fn get(&self, index: VertexIndex) -> Option<&Vertex> {
        match index.type_ {
            VertexType::Position => self.get_position(index.id),
            VertexType::Transition => self.get_transition(index.id)
        }
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
        trace!("remove position at {index}");
        if let Some(position) = self.get_position(index).map(|v| v.index()) {
            self.connections
                .retain(|c| c.first().ne(&position) && c.second().ne(&position));
            self.positions.remove(&position);
            debug!("removed position {position}");
        }
    }

    /// Получить переход по индексу [`VertexIndex`]
    pub fn get_transition(&self, index: usize) -> Option<&Vertex> {
        self.transitions.get(&VertexIndex::transition(index))
    }

    /// Удалить переход по индексу [`VertexIndex`]
    pub fn remove_transition(&mut self, index: usize) {
        trace!("remove transition at {index}");
        if let Some(transition) = self.get_transition(index).map(|v| v.index()) {
            self.connections
                .retain(|c| c.first().ne(&transition) && c.second().ne(&transition));
            self.transitions.remove(&transition);
            debug!("removed transition {transition}");
        }
    }

    /// Удалить элемент по индексу [`VertexIndex`]
    pub fn remove(&mut self, index: VertexIndex) {
        match index.type_ {
            VertexType::Position => self.remove_position(index.id),
            VertexType::Transition => self.remove_transition(index.id)
        }
    }

    /// Добавить позицию по индексу [`VertexIndex`]
    pub fn add_position(&mut self, index: usize) -> &Vertex {
        debug!("add new position p{index}");
        // Если текущее значение меньше, тогда установить новое
        self.position_index.fetch_max(index + 1, Ordering::SeqCst);
        self.positions
            .entry(VertexIndex::position(index))
            .or_insert_with(|| Vertex::position(index))
    }

    /// Добавить переход по индексу [`VertexIndex`]
    pub fn add_transition(&mut self, index: usize) -> &Vertex {
        debug!("add new transition t{index}");
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
        trace!("insert position {}", element.index());
        match self.positions.contains_key(&element.index()) {
            true => {
                debug!("position {} already exists. no inserted", element.index());
                self.positions.get(&element.index()).unwrap()
            },
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
        trace!("insert transition {}", element.index());
        match self.transitions.contains_key(&element.index()) {
            true => {
                debug!("transition {} already exists. no inserted", element.index());
                self.transitions.get(&element.index()).unwrap()
            },
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

    /// Добавляет соединение в структуру сети Петри
    ///
    /// Если соединение существует, то изменяет его вес на новый
    pub fn connect(&mut self, from: VertexIndex, to: VertexIndex, weight: usize) {
        if from.type_ == to.type_ {
            panic!("{} and {} has same type", from, to);
        }

        match self
            .connections
            .iter_mut()
            .find(|c| c.first() == from && c.second() == to)
        {
            Some(connection) => connection.set_weight(weight),
            None => self.connections.push(Connection::new(from, to, weight)),
        }
    }

    /// Удаляет соединение из `from` в `to` и возвращает его
    pub fn disconnect(&mut self, from: VertexIndex, to: VertexIndex) -> Option<Connection> {
        if let Some(index) = self.connections.iter().position(|c| c.first() == from && c.second() == to) {
            Some(self.connections.remove(index))
        } else {
            None
        }
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