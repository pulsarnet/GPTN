use super::{PetriNet, Vertex, VertexIndex, VertexType};
use std::collections::HashMap;
use nalgebra::DMatrix;
use ffi::matrix::CNamedMatrix;
use net::Connection;

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
        self.0
            .sort_by(|net_a, net_b| net_b.positions.len().cmp(&net_a.positions.len()));
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
                let mut from = net
                    .connections
                    .iter()
                    .filter(|c| c.first().eq(&transition.index()));

                while let Some(f) = from.next() {
                    if result.positions.get(&f.second()).is_some() {
                        continue;
                    }

                    let mut to = net.connections.iter().filter(|c| {
                        c.first().ne(&f.second()) && c.second().eq(&transition.index())
                    });

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
                    d_output.column_mut(*transitions.get(&connection.first()).unwrap())
                        [*positions.get(&connection.second()).unwrap()] =
                        connection.weight() as i32;
                }
                _ => {
                    d_input.row_mut(*positions.get(&connection.first()).unwrap())
                        [*transitions.get(&connection.second()).unwrap()] =
                        connection.weight() as i32;
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
        self.0.iter().flat_map(|n| n.connections.iter()).collect()
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
                    d_output.column_mut(*tran_indexes.get(&first).unwrap())
                        [*pos_indexes.get(&second).unwrap()] = connection.weight() as i32;
                }
                _ => {
                    d_input.row_mut(*pos_indexes.get(&first).unwrap())
                        [*tran_indexes.get(&second).unwrap()] = connection.weight() as i32;
                }
            }
        }

        (d_input, d_output)
    }

    pub fn lbf_matrix(&self) -> Vec<(CNamedMatrix, CNamedMatrix)> {
        self.0.iter().map(|net| net.as_matrix()).collect()
    }
}