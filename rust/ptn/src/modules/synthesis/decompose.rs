use std::ops::{AddAssign, MulAssign};
use crate::core::SetPartitionMesh;
use crate::modules::synthesis::SynthesisProgram;
use crate::net::vertex::Vertex;
use crate::net::PetriNet;
use crate::CMatrix;
use nalgebra::base::DMatrix;
use ndarray::{Array1, Array2};
use ndarray_linalg::Solve;
use tracing::{debug, info};
use core::{MatrixExt, NetCycles, NetPaths};
use net::DirectedEdge;
use net::vertex::{VertexIndex, VertexType};

/// Контекст декомпозиции
///
/// Содержит информацию о разделении сети Петри на состовляющие компоненты
/// и сеть Петри в примитивной системе координат

pub struct DecomposeContextBuilder {
    pub parts: PetriNet,
}

impl DecomposeContextBuilder {
    pub fn new(parts: PetriNet) -> Self {
        DecomposeContextBuilder { parts }
    }

    fn solve_with_mu(mut a: Array2<f64>, mut b: Array1<f64>) -> Array1<f64> {
        // find all negative in n/2 equation
        let mu_equation_index = a.nrows() / 2;
        for i in 0..mu_equation_index {
            for j in 0..a.ncols() {
                if a[(i, j)] == -1. && a[(mu_equation_index, j)] != 0. {
                    let multiplier = a[(mu_equation_index, j)];
                    a.row_mut(i).mul_assign(multiplier);
                    b[i] *= multiplier;

                    let a_row = a.row(i).into_owned();
                    let mut mu_row = a.row_mut(mu_equation_index);
                    mu_row.add_assign(&a_row);

                    b[mu_equation_index] += b[i];
                }
            }
        }

        // Set free variable
        let mut free_variable = 0;
        for j in 0..a.ncols() {
            if a[(mu_equation_index, j)] > 0. {
                free_variable = j;
                break;
            }
        }

        // Set other negative to zero
        let mut next_equation_index = mu_equation_index + 1;
        'outer: for i in 0..mu_equation_index {
            for j in 0..a.ncols() {
                if a[(i, j)] == 1. && j == free_variable {
                    continue 'outer;
                }
            }

            for j in 0..a.ncols() {
                if b[i] > 0. && a[(i, j)] > 0. {
                    a[(next_equation_index, j)] = a[(i, j)];
                    b[next_equation_index] = b[i];
                    next_equation_index += 1;
                    break;
                } else if b[i] < 0. && a[(i, j)] < 0. {
                    a[(next_equation_index, j)] = a[(i, j)].abs();
                    b[next_equation_index] -= b[i];
                    next_equation_index += 1;
                    break;
                } else if a[(i, j)] < 0. {
                    a[(next_equation_index, j)] = a[(i, j)].abs();
                    next_equation_index += 1;
                    break;
                }
            }
        }

        debug!("Solve: {a} and {b} =");
        let solution = a.solve(&b).unwrap();
        debug!("{solution}");
        solution
    }

    fn solve_without_mu(mut a: Array2<f64>, mut b: Array1<f64>) -> Array1<f64> {
        let mut mu_equation_index = a.nrows() / 2;
        for i in 0..mu_equation_index {
            for j in 0..a.ncols() {
                if b[i] > 0. && a[(i, j)] == 1. {
                    a[(mu_equation_index, j)] = 1.;
                    b[mu_equation_index] = b[i];
                    mu_equation_index += 1;
                    break;
                } else if b[i] < 0. && a[(i, j)] == -1. {
                    a[(mu_equation_index, j)] = 1.;
                    b[mu_equation_index] -= b[i];
                    mu_equation_index += 1;
                    break;
                } else if a[(i, j)] == -1. {
                    a[(mu_equation_index, j)] = 1.;
                    mu_equation_index += 1;
                    break;
                }

                // if a[(i, j)] == -1. {
                //     a[(mu_equation_index, j)] = 1.;
                //     mu_equation_index += 1;
                // }
            }
        }

        let solution = a.solve(&b).unwrap();
        debug!("Solve: {a} and {b} = {solution}");
        solution
    }

    fn solve(a: Array2<f64>, b: Array1<f64>) -> Array1<f64> {
        let mu_equation_index = a.nrows() / 2;
        let mu_equation = a.row(mu_equation_index);
        if mu_equation.iter().any(|&x| x != 0.) {
            DecomposeContextBuilder::solve_with_mu(a, b)
        } else {
            DecomposeContextBuilder::solve_without_mu(a, b)
        }
    }

    pub fn calculate_c_matrix(
        positions: usize,
        transitions: usize,
        linear_base_fragments: &(DMatrix<f64>, DMatrix<f64>),
        primitive_matrix: DMatrix<f64>,
        mu: &DMatrix<f64>,
    ) -> DMatrix<f64> {
        let mut c_matrix = nalgebra::DMatrix::<f64>::zeros(positions, positions);
        let d_matrix = linear_base_fragments.1.clone() - linear_base_fragments.0.clone();

        for row in 0..positions {
            let mut array_a = Array2::<f64>::zeros((positions, positions));
            let mut array_b = Array1::<f64>::zeros(positions);

            for col in 0..transitions {
                array_a.row_mut(col).assign(
                    &primitive_matrix
                        .column(col)
                        .iter()
                        .copied()
                        .collect::<Array1<f64>>(),
                );
                array_b[col] = d_matrix[(row, col)];
            }

            // set mu equation
            array_a
                .row_mut(transitions)
                .assign(&mu.row(0).iter().copied().collect::<Array1<f64>>());
            array_b[transitions] = mu[(0, row)];

            let solution = DecomposeContextBuilder::solve(array_a, array_b);
            c_matrix
                .row_mut(row)
                .copy_from_slice(solution.as_slice().unwrap());
        }

        debug!("Tensor C: {}", c_matrix);

        c_matrix.iter_mut().for_each(|x| {
            if x.fract() == 0. && *x == 0. {
                *x = 0.
            }
        });

        c_matrix
    }

    pub fn build(self) -> DecomposeContext {
        let mut parts = self.parts;
        let positions = parts.positions().values().cloned().collect::<Vec<_>>();
        let transitions = parts.transitions().values().cloned().collect::<Vec<_>>();

        let primitive_net = primitive_net(&mut parts);
        let adjacency_primitive = primitive_net.one_zero_adjacency_matrices();

        let linear_base_fragments_matrix = parts.one_zero_adjacency_matrices();
        let linear_base_fragments_matrix_f64 = parts.one_zero_adjacency_matrices();
        let mu = DMatrix::from_row_slice(
            1,
            positions.len(),
            &positions
                .iter()
                .map(|x| x.markers() as f64)
                .collect::<Vec<_>>(),
        );

        debug!("input: {} output: {}", adjacency_primitive.0, adjacency_primitive.1);
        debug!("input: {} output: {}", linear_base_fragments_matrix.0, linear_base_fragments_matrix.1);
        let c_matrix = DecomposeContextBuilder::calculate_c_matrix(
            positions.len(),
            transitions.len(),
            &linear_base_fragments_matrix,
            adjacency_primitive.1.clone() - adjacency_primitive.0.clone(),
            &mu,
        );

        let (pos, tran) = (positions.len(), transitions.len());
        DecomposeContext {
            positions,
            transitions,
            primitive_net,
            primitive_matrix: adjacency_primitive,
            linear_base_fragments_matrix: (
                CMatrix::from(linear_base_fragments_matrix_f64.0),
                CMatrix::from(linear_base_fragments_matrix_f64.1),
            ),
            c_matrix,
            programs: SetPartitionMesh::new(pos, tran),
        }
    }
}

#[derive(Debug, Clone)]
pub struct DecomposeContext {
    pub positions: Vec<Vertex>,
    pub transitions: Vec<Vertex>,
    pub primitive_net: PetriNet,
    pub primitive_matrix: (DMatrix<f64>, DMatrix<f64>),
    pub linear_base_fragments_matrix: (CMatrix, CMatrix),

    pub programs: SetPartitionMesh,

    pub c_matrix: DMatrix<f64>,
}

impl DecomposeContext {
    pub fn init(net: &PetriNet) -> Self {
        // TODO check connection types
        let mut net = net.clone();
        let mut parts = vec![];

        parts.extend(extract_loops(&mut net));
        parts.extend(extract_paths(&mut net));

        let mut lbf_net = PetriNet::new();
        for part in parts {
            part.positions().iter().for_each(|(_, v)| { lbf_net.insert(v.clone()); });
            part.transitions().iter().for_each(|(_, v)| { lbf_net.insert(v.clone()); });
            part.directed().iter().for_each(|conn| { lbf_net.add_directed(conn.clone()); });
        }

        normalize_net(&mut lbf_net);

        DecomposeContextBuilder::new(lbf_net).build()
    }

    pub fn marking(&self) -> DMatrix<f64> {
        let mut marking = DMatrix::zeros(self.positions.len(), 1);
        for (i, p) in self.positions.iter().enumerate() {
            marking[(i, 0)] = p.markers() as f64;
        }
        marking
    }

    pub fn linear_base_fragments(&self) -> PetriNet {
        // TODO: Установить максимальный индекс у позиции и перехода
        // TODO: Получение позиции по индексу
        // TODO: Получение перехода по индексу

        let mut result = PetriNet::new();
        let (d_input, d_output) = &self.linear_base_fragments_matrix;

        result
            .positions_mut()
            .extend(self.positions.iter().map(|v| (v.index(), v.clone())));
        result
            .transitions_mut()
            .extend(self.transitions.iter().map(|v| (v.index(), v.clone())));

        for row in 0..d_input.nrows() {
            for column in 0..d_input.ncols() {
                if d_input.row(row)[column] > 0 {
                    result.add_directed(
                        DirectedEdge::new(
                            self.positions
                                .iter()
                                .enumerate()
                                .find(|(k, _)| *k == row)
                                .map(|(_, k)| k.index())
                                .unwrap(),
                            self.transitions
                                .iter()
                                .enumerate()
                                .find(|(k, _)| *k == column)
                                .map(|(_, k)| k.index())
                                .unwrap(),
                        )
                    )
                }

                if d_output.column(column)[row] > 0 {
                    result.add_directed(
                        DirectedEdge::new(
                            self.transitions
                                .iter()
                                .enumerate()
                                .find(|(k, _)| *k == column)
                                .map(|(_, k)| k.index())
                                .unwrap(),
                            self.positions
                                .iter()
                                .enumerate()
                                .find(|(k, _)| *k == row)
                                .map(|(_, k)| k.index())
                                .unwrap(),
                        )
                    )
                }
            }
        }

        result
    }

    pub fn positions(&self) -> &Vec<Vertex> {
        &self.positions
    }

    pub fn transitions(&self) -> &Vec<Vertex> {
        &self.transitions
    }

    pub fn programs(&self) -> &SetPartitionMesh {
        &self.programs
    }

    pub fn program_value(&self, _: usize, _: usize) -> usize {
        //self.programs()[program].data[index] as usize
        0
    }

    pub fn set_program_value(&mut self, _: usize, _: usize, _: usize) {
        //self.programs[program].data[index] = value as u16;
    }

    pub fn program_equation(&self, index: usize) -> String {
        let pos_indexes_vec = self.positions();
        let tran_indexes_vec = self.transitions();

        let program =
            SynthesisProgram::new_with(self.programs.get_partition(index), tran_indexes_vec.len());

        let (t_sets, p_sets) = program.sets();

        info!(
            "{:?}\n{:?}\n{:?}\n{:?}",
            t_sets,
            p_sets,
            pos_indexes_vec,
            tran_indexes_vec
        );

        let mut result = String::new();
        for set in t_sets {
            if set.is_empty() {
                continue;
            }

            result += tran_indexes_vec[*set.last().unwrap()].name().as_str();
            result += " = ";
            for i in (0..set.len()).rev() {
                result += &tran_indexes_vec[set[i]].name();
                if i > 0 {
                    result += " + ";
                }
            }

            result += "\n";
        }

        for set in p_sets {
            if set.is_empty() {
                continue;
            }

            result += &pos_indexes_vec[*set.last().unwrap()].name().as_str();
            result += " = ";
            for i in (0..set.len()).rev() {
                result += &pos_indexes_vec[set[i]].name();
                if i > 0 {
                    result += " + ";
                }
            }

            result += "\n";
        }

        result
    }

    pub fn program_header_name(&self, index: usize, label: bool) -> String {
        if index < self.transitions().len() {
            match label {
                true => self.transitions()[index].label(false),
                false => self.transitions()[index].full_name(),
            }
        } else {
            match label {
                true => self.positions()[index - self.transitions().len()].label(false),
                false => self.positions()[index - self.transitions().len()].full_name(),
            }
        }
    }

    pub fn primitive_net(&self) -> &PetriNet {
        &self.primitive_net
    }

    pub fn transition_synthesis_program(
        &self,
        t_set: &Vec<usize>,
        adjacency_input: &mut DMatrix<f64>,
        adjacency_output: &mut DMatrix<f64>,
    ) {
        assert!(t_set.len() > 1);

        let first = t_set[0];
        for &t in t_set.iter().skip(1) {
            adjacency_input.logical_column_add(first, t);
            adjacency_output.logical_column_add(first, t);
            // logical_column_add(adjacency_input, first, t);
            // logical_column_add(adjacency_output, first, t);
        }

        for &t in t_set.iter().skip(1) {
            adjacency_input.logical_column_add(t, first);
            adjacency_output.logical_column_add(t, first);
        }
    }

    pub fn position_synthesis_program(
        &self,
        p_set: &Vec<usize>,
        adjacency_input: &mut DMatrix<f64>,
        adjacency_output: &mut DMatrix<f64>,
        d_markers: &mut DMatrix<f64>,
    ) {
        let first = p_set[0];
        for p in p_set.iter().skip(1) {
            adjacency_input.logical_row_add(first, *p);
            adjacency_output.logical_row_add(first, *p);
            // logical_row_add(adjacency_input, first, *p);
            // logical_row_add(adjacency_output, first, *p);
            d_markers[(first, 0)] = d_markers[(*p, 0)].max(d_markers[(first, 0)]);
        }

        for p in p_set.iter().skip(1) {
            // logical_row_add(adjacency_input, *p, first);
            // logical_row_add(adjacency_output, *p, first);
            d_markers[(*p, 0)] = d_markers[(first, 0)];
        }
    }
}

/// Нормализует линейно-базовый фрагмент так, чтобы
/// позиции находящиеся между двумя переходами дублировались
fn normalize_net(net: &mut PetriNet) {
    let mut positions = vec![];
    let mut connections = vec![];
    for (&idx, position) in net.positions() {
        // Проверим, что позиция не является входной или выходной, т.е. находится между двумя переходами
        let Some(input) = net.directed().iter().find(|conn| conn.end() == idx) else { continue };
        let Some(output) = net.directed().iter().find(|conn| conn.begin() == idx) else { continue };

        // Создадим новую позицию с родителем
        let new_pos = position.split(net.next_position_index());
        let new_pos_index = new_pos.index();
        positions.push(new_pos);

        // Соединим новую позицию с переходами
        connections.push(DirectedEdge::new_with(input.begin(), new_pos_index, input.weight()));
        connections.push(DirectedEdge::new_with(new_pos_index, output.end(), output.weight()));
    }

    for pos in positions.into_iter() {
        // Добавим позиции в сеть
        net.insert(pos);
    }

    for conn in connections.into_iter() {
        // Добавим соединения
        net.add_directed(conn);
    }
}

/// Удаляет вершины из сети Петри, таким образом, что если
/// на вершину ссылаются элементы не входящие в массив удаляемых вершин, то она дублируется
///
/// # Important
/// Вершины в массиве должны чередоваться T/P или P/T
///
/// # Возвращает
/// [`PetriNet`] - извлеченная сеть
fn extract_part(net: &mut PetriNet, remove: &[VertexIndex]) -> PetriNet {
    debug!("remove part {remove:?}");
    let mut result = PetriNet::new();

    // Добавим вершины в новую сеть
    for &index in remove {
        result.insert(net.get(index).unwrap().clone());
    }

    // Соединим добавленные вершины и удалим соединения из текущей сети
    for connect in remove.windows(2) {
        assert_ne!(connect[0].type_, connect[1].type_, "vertices must be different types");
        // удалим соединение в текущей сети
        let removed = net.remove_directed(connect[0], connect[1])
            .expect("BUG: the connection must exists in net");
        result.add_directed(removed);
    }

    // Проверим, что это цикл: если типы 1 элемента и последнего неравны, то это цикл
    if remove[0].type_ != remove[remove.len() - 1].type_ {
        let removed = net.remove_directed(remove[remove.len() - 1], remove[0])
            .expect("BUG: the connection must exists in net");
        result.add_directed(removed)
    }

    // Для каждого удаляемого элемента проверим:
    // если остались соединения, тогда дублируем элемент и соединения и затем удаляем
    // TODO: По хорошему в найденном цикле могут быть обратные дуги, их бы тоже удалить (ЗАЧЕМ? ОБОСНОВАНИЕ?).
    // TODO: то есть для элемента могли остаться обратные соединения
    for &index in remove {
        let found = net.directed().iter()
            .find(|conn| conn.begin() == index || conn.end() == index)
            .is_some();

        // Если нашли, что у элемента есть соединение, то делим его и потом удаляем родительский элемент
        if found {
            let split = split_element(net, index);

            // Когда делимый элемент является переходом, то проверяем, чтобы из него выходило соединение
            // В ином случае необходимо вернуть позицию в которую входит родительский элемент и тоже её разделить
            if VertexType::Transition == split.type_ {
                if let None = net.directed().iter().find(|conn| conn.begin() == index) {
                    let connection = result.directed().iter().find(|conn| conn.begin() == index)
                        .copied()
                        .expect("an output position must exists");

                    let position = result.get(connection.end())
                        .expect("an position must exists")
                        .split(net.next_position_index());

                    let idx = net.insert(position).index();
                    net.add_directed(DirectedEdge::new_with(split, idx, connection.weight()));
                }
            }
        }

        net.remove(index);
    }

    result
}

// Делит элемент в сети на 2 элемента, также копирует соединения
fn split_element(net: &mut PetriNet, index: VertexIndex) -> VertexIndex {
    let element = net.get(index)
        .expect("when this method call, net always contains element by index");

    let new_element = match index.type_ {
        VertexType::Position => element.split(net.next_position_index()),
        VertexType::Transition => element.split(net.next_transition_index())
    };
    let new_element_index = new_element.index();
    net.insert(new_element); // добавляем новый элемент

    // Скопировать соединения
    let connections = net.directed().iter()
        .filter_map(|conn| {
            if conn.begin() == index {
                Some(DirectedEdge::new_with(new_element_index, conn.end(), conn.weight()))
            } else if conn.end() == index {
                Some(DirectedEdge::new_with(conn.begin(), new_element_index, conn.weight()))
            } else {
                None
            }
        })
        .collect::<Vec<_>>();

    for conn in connections {
        net.add_directed(conn);
    }

    new_element_index
}

/// Удаляет из сети Петри все циклы (начиная с самого большого)
fn extract_loops(net: &mut PetriNet) -> Vec<PetriNet> {
    let mut loops = vec![];
    loop {
        let net_loops = NetCycles::find(net);
        match net_loops.get_longest() {
            None => break,
            Some(l) => loops.push(extract_part(net, l)),
        }
    }
    loops
}

/// Удаляет из сети Петри все прямые пути (начиная с самого большого)
fn extract_paths(net: &mut PetriNet) -> Vec<PetriNet> {
    let mut paths = vec![];
    loop {
        let net_paths = NetPaths::find(net);
        match net_paths.get_longest() {
            None => break,
            Some(l) => paths.push(extract_part(net, l)),
        }
    }
    paths
}

fn primitive_net(net: &mut PetriNet) -> PetriNet {
    let mut result = PetriNet::new();

    let transitions = net.transitions();

    'brk: for transition in transitions.values() {
        let mut from = net
            .directed()
            .iter()
            .filter(|c| c.begin().eq(&transition.index()));

        while let Some(from_t) = from.next() {
            if result.positions().get(&from_t.end()).is_some() {
                continue;
            }

            let mut to = net.directed().iter().filter(|c| {
                c.begin().ne(&from_t.end()) && c.end().eq(&transition.index())
            });

            while let Some(to_t) = to.next() {
                if result.positions().get(&to_t.begin()).is_some() {
                    continue;
                }

                result.insert(net.get(to_t.begin()).unwrap().clone());
                result.insert(net.get(from_t.begin()).unwrap().clone());
                result.insert(net.get(from_t.end()).unwrap().clone());

                result.add_directed(DirectedEdge::new(to_t.begin(), from_t.begin().clone())); // to_t.weight()
                result.add_directed(DirectedEdge::new(from_t.begin(), from_t.end().clone())); // from_t.weight()
                continue 'brk;
            }
        }
    }

    result
}


#[cfg(test)]
mod tests {
    use modules::synthesis::decompose::{extract_loops, extract_paths};
    use net::{DirectedEdge, PetriNet, Vertex};
    use net::vertex::VertexIndex;

    #[test]
    fn test_extract_path_1() {
        // p1 -> t1 -> p2
        //        \->  p3

        let mut net = PetriNet::new();
        let p1 = net.add_position(1).index();
        let t1 = net.add_transition(1).index();
        net.add_directed(DirectedEdge::new(p1, t1));

        let p2 = net.add_position(2).index();
        let p3 = net.add_position(3).index();
        net.add_directed(DirectedEdge::new(t1, p2));
        net.add_directed(DirectedEdge::new(t1, p3));

        let loops = extract_loops(&mut net);
        assert!(loops.is_empty());

        let paths = extract_paths(&mut net);
        dbg!(&paths);
        assert_eq!(paths.len(), 2);

        // todo дописать
        let path1 = &paths[0];
        assert_eq!(path1.positions().len(), 2);
        assert_eq!(path1.transitions().len(), 1);
        assert_eq!(path1.directed().len(), 2);
        assert_eq!(path1.get(p1).cloned(), Some(Vertex::position(1)));
        assert_eq!(path1.get(t1).cloned(), Some(Vertex::transition(1)));
        assert_eq!(path1.get(p3).cloned(), Some(Vertex::position(3)));
    }
}