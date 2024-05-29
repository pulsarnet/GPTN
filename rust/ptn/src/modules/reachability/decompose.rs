use nalgebra::DMatrix;
use net::DirectedEdge;
use crate::net::{PetriNet, Vertex};

pub struct PrimitiveDecomposition {
    net: PetriNet,
    primitive_net: PetriNet,
    input_tensor: DMatrix<i32>,
    output_tensor: DMatrix<i32>,
    inverse_input_tensor: DMatrix<i32>,
    inverse_output_tensor: DMatrix<i32>,
}

impl PrimitiveDecomposition {
    pub fn new(mut net: PetriNet) -> Result<Self, ()> {
        let positions_count = net.positions().len();
        let transitions_count = net.transitions().len();

        if positions_count > 2 * transitions_count {
            return Err(());
        }

        // Добавим эквивалентные позиции
        let mut need = 2 * transitions_count - positions_count;
        if need > 0 {
            // Получим внутренние позиции
            let filter = net
                .positions()
                .iter()
                .filter(|(index, _)| {
                    net.directed()
                        .iter()
                        .find(|connection| connection.begin().eq(index))
                        .is_some()
                        && net
                            .directed()
                            .iter()
                            .find(|connection| connection.end().eq(index))
                            .is_some()
                })
                .take(need)
                .map(|(index, position)| (index.clone(), position.clone()))
                .collect::<Vec<_>>();

            if filter.len() < need {
                return Err(());
            }

            let mut iter = filter.iter();

            while need > 0 {
                let Some((index, _)) = iter.next() else { break };
                let position_index = net.next_position_index();

                let new_position = Vertex::position(position_index);
                let new_index = new_position.index();

                net.insert_position(new_position);
                let connections = net
                    .directed()
                    .iter()
                    .filter(|connection| {
                        connection.begin().eq(index) || connection.end().eq(index)
                    })
                    .cloned()
                    .collect::<Vec<_>>();

                connections.into_iter().for_each(|connection| {
                    if connection.begin().eq(index) {
                        net.add_directed(DirectedEdge::new_with(new_index, connection.end(), connection.weight()));
                    } else {
                        net.add_directed(DirectedEdge::new_with(connection.begin(), new_index, connection.weight()));
                    }
                });

                need -= 1;
            }

            if need > 0 {
                return Err(());
            }
        }

        // Создадим матрицы D(I) и D(O)
        let (d_input, d_output) = net.adjacency_matrices::<i32>();

        // Получим общую матрицу инцидентности D
        let d_matrix = d_output - d_input;

        // Создадим примитивную систему
        let Some(primitive_matrix) = simplify_matrix(d_matrix) else { return Err(()) };
        let mut primitive_net = PetriNet::new();
        net.positions().iter().for_each(|(_, vertex)| {
            primitive_net.insert_position(vertex.clone());
        });

        net.transitions().iter().for_each(|(_, vertex)| {
            primitive_net.insert_transition(vertex.clone());
        });

        for row_index in 0..primitive_matrix.nrows() {
            for column_index in 0..primitive_matrix.ncols() {
                let value = primitive_matrix.row(row_index)[column_index];
                if value > 0 {
                    primitive_net.add_directed(
                        DirectedEdge::new_with(
                            net.transitions()[column_index].index(),
                            net.positions()[row_index].index(),
                            value as u32
                        )
                    )
                } else if value < 0 {
                    primitive_net.add_directed(
                        DirectedEdge::new_with(
                            net.transitions()[column_index].index(),
                            net.positions()[row_index].index(),
                            (-value) as u32
                        )
                    )
                }
            }
        }

        let c_input = DMatrix::<i32>::zeros(net.positions().len(), net.positions().len());
        let c_output = DMatrix::<i32>::zeros(net.positions().len(), net.positions().len());
        // Вычислим тензоры C(I) и C(O)
        // let (c_input, c_output) = DecomposeContextBuilder::calculate_c_matrix2(
        //     &PetriNetVec(vec![primitive_net.clone()])
        // );

        // Вычислим тензоры E(I) и E(O)
        let Some(e_input) = c_input.map(|i| i as f64).try_inverse()
            .map(|matrix| matrix.map(|i| i as i32))
            else { return Err(()); };

        let Some(e_output) = c_output.map(|i| i as f64).try_inverse()
            .map(|matrix| matrix.map(|i| i as i32))
            else { return Err(()); };

        Ok(Self {
            net,
            primitive_net,
            input_tensor: c_input,
            output_tensor: c_output,
            inverse_input_tensor: e_input,
            inverse_output_tensor: e_output,
        })
    }
}

fn khun_algorithnm(
    graph: &Vec<Vec<usize>>,
    mt: &mut Vec<i32>,
    used: &mut Vec<bool>,
    vertex: usize,
) -> bool {
    if used[vertex] {
        return false;
    }

    used[vertex] = true;
    for i in 0..graph[vertex].len() {
        let to = graph[vertex][i];
        if mt[to] == -1 || khun_algorithnm(graph, mt, used, mt[to] as usize) {
            mt[to] = vertex as i32;
            return true;
        }
    }

    return false;
}

fn simplify_matrix(matrix: DMatrix<i32>) -> Option<DMatrix<i32>> {
    // // Объявляем правильную матрицу с -1 и 1
    // let mut correct_matrix = DMatrix::from_element(matrix.nrows(), matrix.ncols(), 0);
    // for row in 0..correct_matrix.nrows() {
    //     for col in 0..correct_matrix.ncols() {
    //         if row == col * 2 + 1 {
    //             correct_matrix[(row, col)] = 1;
    //         } else if row == col * 2 {
    //             correct_matrix[(row, col)] = -1;
    //         }
    //     }
    // }

    // Объявить двумерный массив candidates[n][]
    //

    let mut candidate_rows = vec![Vec::new(); matrix.nrows()];
    for row in 0..matrix.nrows() {
        for col in 0..matrix.ncols() {
            if matrix[(row, col)] == 1 {
                candidate_rows[row].push(2 * col + 1)
            } else if matrix[(row, col)] == -1 {
                candidate_rows[row].push(2 * col)
            }
        }
    }

    // let mut candidate_rows = vec![Vec::new(); matrix.nrows()];
    // for row in 0..matrix.nrows() {
    //     for candidate_row in 0..correct_matrix.nrows() {
    //         for col in 0..matrix.ncols() {
    //             if correct_matrix[(candidate_row, col)] != 0 && matrix[(row, col)] == correct_matrix[(candidate_row, col)] {
    //                 candidate_rows[row].push(candidate_row);
    //
    //                 break;
    //             }
    //         }
    //     }
    // }
    //
    // println!("Matrix: {}", correct_matrix);
    println!("Candidate rows: {:?}", candidate_rows);

    // let mut result_rows = vec![];
    // let mut counter = 0;
    // recursive_choice_not_in_choice(&mut candidate_rows, &mut result_rows, 0, &mut counter);

    // Khun solution
    let mut mt = vec![-1; matrix.nrows()];
    let mut used = vec![false; matrix.nrows()];
    for i in 0..matrix.nrows() {
        used.fill(false);
        khun_algorithnm(&candidate_rows, &mut mt, &mut used, i);
    }

    // println!("Count: {}", counter);

    if mt.iter().any(|&x| x == -1) {
        println!("Bad mt: {:?}", mt);
        return None;
    }

    println!("Result rows: {:?}", mt);

    // i = 2 * j + 1 => j = (i - 1) / 2
    // i = 2 * j => j = i / 2
    let mut result_matrix = DMatrix::zeros(matrix.nrows(), matrix.ncols());
    for (index, row) in mt.iter().enumerate() {
        if index % 2 == 0 {
            result_matrix[(*row as usize, index / 2)] = -1;
        } else {
            result_matrix[(*row as usize, (index - 1) / 2)] = 1;
        }
        //result_matrix.set_row(*row as usize, &correct_matrix.row(index));
    }

    Some(result_matrix)
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_simplify_matrix() {
        let matrix = DMatrix::from_row_slice(
            6,
            3,
            &[-1, 0, 0, 1, 1, 1, 1, -1, 0, 0, -1, -1, 0, 1, 0, 0, 0, 1],
        );

        let result = simplify_matrix(matrix);
        println!("{}", result.clone().unwrap());
        assert!(result.is_some());
    }
}
