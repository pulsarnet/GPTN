use crate::core::SetPartitionMesh;
use crate::core::{logical_column_add, logical_row_add};
use crate::modules::synthesis::SynthesisProgram;
use crate::net::vertex::Vertex;
use crate::net::{PetriNet, PetriNetVec};
use crate::CMatrix;
use nalgebra::base::DMatrix;
use ndarray::{Array1, Array2};
use ndarray_linalg::Solve;
use tracing::info;

/// Контекст декомпозиции
///
/// Содержит информацию о разделении сети Петри на состовляющие компоненты
/// и сеть Петри в примитивной системе координат

pub struct DecomposeContextBuilder {
    pub parts: PetriNetVec,
}

impl DecomposeContextBuilder {
    pub fn new(parts: PetriNetVec) -> Self {
        DecomposeContextBuilder { parts }
    }

    fn solve_with_mu(mut a: Array2<f64>, mut b: Array1<f64>) -> Array1<f64> {
        // find all negative in n/2 eqution
        let mu_equation_index = a.nrows() / 2;
        for i in 0..mu_equation_index {
            for j in 0..a.ncols() {
                if a[(i, j)] == -1. && a[(mu_equation_index, j)] != 0. {
                    a[(mu_equation_index, j)] = 0.;
                    for k in 0..a.ncols() {
                        if a[(i, k)] == 1. {
                            a[(mu_equation_index, k)] += 1.;
                            b[mu_equation_index] += b[i];
                        }
                    }
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
                if b[i] > 0. && a[(i, j)] == 1. {
                    a[(next_equation_index, j)] = 1.;
                    b[next_equation_index] = b[i];
                    next_equation_index += 1;
                    break;
                } else if b[i] < 0. && a[(i, j)] == -1. {
                    a[(next_equation_index, j)] = 1.;
                    b[next_equation_index] -= b[i];
                    next_equation_index += 1;
                    break;
                } else if a[(i, j)] == -1. {
                    a[(next_equation_index, j)] = 1.;
                    next_equation_index += 1;
                    break;
                }
                // if a[(i, j)] == -1.{
                //     a[(next_equation_index, j)] = 1.;
                //     next_equation_index += 1;
                // }
            }
        }

        println!("a: {}", a);
        // solve
        a.solve(&b).unwrap()
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

        a.solve(&b).unwrap()
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
        linear_base_fragments: &(DMatrix<i32>, DMatrix<i32>),
        primitive_matrix: DMatrix<i32>,
        mu: &DMatrix<i32>,
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
                        .map(|&x| x as f64)
                        .collect::<Array1<f64>>(),
                );
                array_b[col] = d_matrix[(row, col)] as f64;
            }

            // set mu equation
            array_a
                .row_mut(transitions)
                .assign(&mu.row(0).iter().map(|&x| x as f64).collect::<Array1<f64>>());
            array_b[transitions] = mu[(0, row)] as f64;

            let solution = DecomposeContextBuilder::solve(array_a, array_b);
            c_matrix
                .row_mut(row)
                .copy_from_slice(solution.as_slice().unwrap());
        }

        println!("{}", c_matrix);

        c_matrix.iter_mut().for_each(|x| {
            if x.fract() == 0. && *x == 0. {
                *x = 0.
            }
        });

        println!("{}", c_matrix);

        c_matrix
    }

    pub fn build(mut self) -> DecomposeContext {
        self.parts.sort();

        let parts = self.parts;
        let positions = parts
            .0
            .iter()
            .flat_map(|net| net.positions().values())
            .cloned()
            .collect::<Vec<_>>();
        let transitions = parts
            .0
            .iter()
            .flat_map(|net| net.transitions().values())
            .cloned()
            .collect::<Vec<_>>();

        let primitive_net = parts.primitive_net();
        //let adjacency_primitive = primitive_net.adjacency_matrix();
        let adjacency_primitive = primitive_net.adjacency_matrices();
        println!("adjacency_primitive 0: {}", adjacency_primitive.0);
        println!("adjacency_primitive 1: {}", adjacency_primitive.1);

        let (primitive_input, primitive_output) = parts.primitive_matrix();
        let linear_base_fragments_matrix = parts.equivalent_matrix();
        let mu = DMatrix::from_row_slice(
            1,
            positions.len(),
            &positions
                .iter()
                .map(|x| x.markers() as i32)
                .collect::<Vec<_>>(),
        );
        let c_matrix = DecomposeContextBuilder::calculate_c_matrix(
            positions.len(),
            transitions.len(),
            &linear_base_fragments_matrix,
            primitive_output.clone() - primitive_input.clone(),
            &mu,
        );

        let (pos, tran) = (positions.len(), transitions.len());
        DecomposeContext {
            parts,
            positions,
            transitions,
            primitive_net,
            primitive_matrix: adjacency_primitive,
            linear_base_fragments_matrix: (
                CMatrix::from(linear_base_fragments_matrix.0),
                CMatrix::from(linear_base_fragments_matrix.1),
            ),
            c_matrix,
            programs: SetPartitionMesh::new(pos, tran),
        }
    }
}

#[derive(Debug, Clone)]
pub struct DecomposeContext {
    pub parts: PetriNetVec,
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
        let mut net = net.clone();
        let mut parts = vec![];

        while let Some(l) = net.get_loop() {
            parts.push(net.remove_part(&l));
        }

        while let Some(p) = net.get_part() {
            parts.push(net.remove_part(&p));
        }

        parts.iter_mut().for_each(|net| net.normalize());

        let parts = PetriNetVec(parts);

        DecomposeContextBuilder::new(parts).build()
        //res.add_synthesis_programs();
        //res
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
                    result.connect(
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
                }

                if d_output.column(column)[row] > 0 {
                    result.connect(
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

        let (t_sets, p_sets) = program.sets(pos_indexes_vec, tran_indexes_vec);

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
            logical_column_add(adjacency_input, first, t);
            logical_column_add(adjacency_output, first, t);
        }

        for &t in t_set.iter().skip(1) {
            logical_column_add(adjacency_input, t, first);
            logical_column_add(adjacency_output, t, first);
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
            logical_row_add(adjacency_input, first, *p);
            logical_row_add(adjacency_output, first, *p);
            d_markers[(first, 0)] = d_markers[(*p, 0)].max(d_markers[(first, 0)]);
        }

        for p in p_set.iter().skip(1) {
            logical_row_add(adjacency_input, *p, first);
            logical_row_add(adjacency_output, *p, first);
            d_markers[(*p, 0)] = d_markers[(first, 0)];
        }
    }
}
