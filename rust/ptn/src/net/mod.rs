pub mod vertex;
mod edge;

use indexmap::map::IndexMap;
use nalgebra::{DMatrix, Scalar};
pub use net::vertex::Vertex;
use net::vertex::{VertexIndex, VertexType};
use std::fmt::Debug;
use std::sync::atomic::{AtomicUsize, Ordering};
use num_traits::{AsPrimitive, One, Zero};
use tracing::{debug, trace};
pub use net::edge::{DirectedEdge, Edges, InhibitorEdge};

#[derive(Debug)]
pub struct PetriNet {
    /// Позиции сети Петри
    positions: IndexMap<VertexIndex, Vertex>,

    /// Переходы сети Петри
    transitions: IndexMap<VertexIndex, Vertex>,

    /// Соединения между вершинами в сети Петри
    edges: Edges,

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

    pub fn edges(&self) -> &Edges {
        &self.edges
    }

    /// Получить элемент по индексу [`VertexIndex`]
    pub fn get(&self, index: VertexIndex) -> Option<&Vertex> {
        match index.type_ {
            VertexType::Position => self.get_position(index.id),
            VertexType::Transition => self.get_transition(index.id)
        }
    }

    /// Установить разметку
    pub fn set_marking(&mut self, index: VertexIndex, marking: usize) {
        self.positions.get_mut(&index)
            .unwrap()
            .set_markers(marking);
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
            self.edges.remove_relative_to_vertex(position);
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
            self.edges.remove_relative_to_vertex(transition);
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
                debug!("transition {} already exists", element.index());
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
    
    pub fn directed(&self) -> &[DirectedEdge] {
        self.edges.directed()
    }
    
    /// Find directed arc by `from` and `to` vertex 
    pub fn get_directed(&self, from: VertexIndex, to: VertexIndex) -> Option<&DirectedEdge> {
        self.edges.get_directed(from, to)
    }

    pub fn add_directed(&mut self, edge: DirectedEdge) {
        self.edges.add_directed(edge)
    }
    
    pub fn remove_directed(&mut self, from: VertexIndex, to: VertexIndex) -> Option<DirectedEdge> {
        self.edges.remove_directed(from, to)
    }

    pub fn get_inhibitor(&self, place: VertexIndex, transition: VertexIndex) -> Option<&InhibitorEdge> {
        self.edges.get_inhibitor(place, transition)
    }

    pub fn add_inhibitor(&mut self, edge: InhibitorEdge) {
        self.edges.add_inhibitor(edge)
    }
    
    pub fn remove_inhibitor(&mut self, place: VertexIndex, transition: VertexIndex) {
        self.edges.remove_inhibitor(place, transition)
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
    
    pub fn adjacency_matrices<I>(&self) -> (DMatrix<I>, DMatrix<I>) 
        where
            I: Scalar + Zero + Copy + 'static,
            u32: AsPrimitive<I>,
    {
        let mut input = DMatrix::<I>::zeros(self.positions.len(), self.transitions.len());
        let mut output = DMatrix::<I>::zeros(self.positions.len(), self.transitions.len());

        for conn in self.edges.directed() {
            let begin = conn.begin();
            let end = conn.end();
            
            match begin.type_ {
                VertexType::Transition => {
                    let p_idx = self.positions.get_index_of(&end).unwrap();
                    let t_idx = self.transitions.get_index_of(&begin).unwrap();

                    output[(p_idx, t_idx)] = conn.weight().as_();
                }
                VertexType::Position => {
                    let p_idx = self.positions.get_index_of(&begin).unwrap();
                    let t_idx = self.transitions.get_index_of(&end).unwrap();

                    input[(p_idx, t_idx)] = conn.weight().as_();
                }
            }
        }

        (input, output)
    }
    
    pub fn inhibitor_matrix<I>(&self) -> DMatrix<I>
        where
            I: Scalar + Zero + Copy + 'static,
            u32: AsPrimitive<I>,
    {
        let mut inhibitor = DMatrix::<I>::zeros(self.positions.len(), self.transitions.len());
        
        for arc in self.edges.inhibitor() {
            let place = arc.place();
            let transition = arc.transition();
            
            let p_idx = self.positions.get_index_of(&place).unwrap();
            let t_idx = self.transitions.get_index_of(&transition).unwrap();
            
            inhibitor[(p_idx, t_idx)] = 1u32.as_();
        }
        
        inhibitor
    }

    pub fn one_zero_adjacency_matrices<T>(&self) -> (DMatrix<T>, DMatrix<T>)
        where
            T: Debug + PartialEq + Scalar + Clone + Zero + One
    {
        let mut input = DMatrix::<T>::zeros(self.positions.len(), self.transitions.len());
        let mut output = DMatrix::<T>::zeros(self.positions.len(), self.transitions.len());

        for conn in self.edges.directed() {
            let begin = conn.begin();
            let end = conn.end();

            match begin.type_ {
                VertexType::Transition => {
                    let p_idx = self.positions.get_index_of(&end).unwrap();
                    let t_idx = self.transitions.get_index_of(&begin).unwrap();

                    output[(p_idx, t_idx)] = T::one();
                }
                VertexType::Position => {
                    let p_idx = self.positions.get_index_of(&begin).unwrap();
                    let t_idx = self.transitions.get_index_of(&end).unwrap();

                    input[(p_idx, t_idx)] = T::one();
                }
            }
        }

        (input, output)
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
}

impl Default for PetriNet {
    fn default() -> Self {
        PetriNet {
            positions: IndexMap::new(),
            transitions: IndexMap::new(),
            edges: Edges::default(),
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
            edges: self.edges.clone(),
            position_index: AtomicUsize::new(self.position_index.load(Ordering::SeqCst)),
            transition_index: AtomicUsize::new(self.transition_index.load(Ordering::SeqCst)),
        }
    }
}

#[cfg(test)]
mod tests {


}