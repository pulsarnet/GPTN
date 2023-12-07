use nalgebra::DMatrix;
use num::integer::gcd;
use std::io::Write;

use net::PetriNet;

#[derive(Default, Debug)]
struct PositiveNegative {
    row: usize,
    p_cols: Vec<usize>,
    n_cols: Vec<usize>,
}

impl PositiveNegative {
    pub fn new(mat: &DMatrix<i32>) -> Option<PositiveNegative> {
        let mut tmp = None;
        for (i, row) in mat.row_iter().enumerate() {
            let (pos, neg) = PositiveNegative::partition_pos_neg(row.iter());
            if pos.is_empty() ^ neg.is_empty() {
                tmp = Some(PositiveNegative {
                    row: i,
                    p_cols: pos,
                    n_cols: neg,
                });
                break;
            } else if tmp.is_none() && (!pos.is_empty() || !neg.is_empty()) {
                tmp = Some(PositiveNegative {
                    row: i,
                    p_cols: pos,
                    n_cols: neg,
                })
            }
        }

        tmp
    }

    pub fn min_index(&self) -> usize {
        self.p_cols.first().map_or(self.n_cols[0], |&value| {
            std::cmp::min(value, self.n_cols[0])
        })
    }

    fn partition_pos_neg<'a>(iter: impl Iterator<Item = &'a i32>) -> (Vec<usize>, Vec<usize>) {
        iter.enumerate().filter(|(_, el)| **el != 0).fold(
            (vec![], vec![]),
            |(mut v1, mut v2), (i, &el)| {
                if el > 0 {
                    v1.push(i)
                } else {
                    v2.push(i)
                }

                (v1, v2)
            },
        )
    }
}


pub fn t_invariant(net: &PetriNet) {
    let (i, o) = net.adjacency_matrices();
    let mat = o + i;
    let mat =
        DMatrix::<i32>::from_iterator(mat.nrows(), mat.ncols(), mat.iter().map(|el| *el as i32));

    let res = invariant(mat);
    let transitions = net
        .transitions()
        .values()
        .fold(vec![], |mut acc, transition| {
            acc.push(format!("t{}", transition.index().id));
            acc
        });

    print_matrix(&res, &transitions);
}

pub fn p_invariant(net: &PetriNet) {
    let (i, o) = net.adjacency_matrices();
    let mat = o + i;
    let mat =
        DMatrix::<i32>::from_iterator(mat.nrows(), mat.ncols(), mat.iter().map(|el| *el as i32));

    let res = invariant(mat.transpose());
    let positions = net.positions().values().fold(vec![], |mut acc, pos| {
        acc.push(format!("p{}", pos.index().id));
        acc
    });

    print_matrix(&res, &positions);
}

fn invariant(mut mat_c: DMatrix<i32>) -> DMatrix<i32> {
    let mut mat_b = DMatrix::<i32>::identity(mat_c.ncols(), mat_c.ncols());

    // phase 1:
    while let Some(pn) = PositiveNegative::new(&mat_c) {
        // while In matC exists non-zero row
        if pn.p_cols.is_empty() ^ pn.n_cols.is_empty() {
            // Существует строка в которой не пустое только одно множество (положительные и отрицательные) элементов
            // Тогда удалить все колонки из matC и matB, которые есть в объединении p_cols U n_cols
            let remove = pn
                .p_cols
                .iter()
                .chain(pn.n_cols.iter())
                .copied()
                .collect::<Vec<_>>();

            mat_c = mat_c.remove_columns_at(&remove);
            mat_b = mat_b.remove_columns_at(&remove);
        } else {
            if pn.p_cols.len() == 1 || pn.n_cols.len() == 1 {
                let (k, indexes) = match pn.p_cols.len() == 1 {
                    true => (pn.p_cols[0], pn.n_cols.as_slice()),
                    false => (pn.n_cols[0], pn.p_cols.as_slice()),
                };

                for &index in indexes {
                    let chk = mat_c[(pn.row, k)].abs();
                    let chj = mat_c[(pn.row, index)].abs();
                    for mut row in mat_c.row_iter_mut() {
                        row[index] = row[index] * chk + row[k] * chj;
                    }
                    for mut row in mat_b.row_iter_mut() {
                        row[index] = row[index] * chk + row[k] * chj;
                    }
                }

                mat_c = mat_c.remove_column(k);
                mat_b = mat_b.remove_column(k);
            } else {
                // [1.1.b.1] let h be the index of a non-zero row of C;
                // let k be the index of a column such that chk 0;
                let h = pn.row; //matC.row_iter().position(|row| row.iter().any(|v| *v != 0)).unwrap();
                let k = pn.min_index(); //matC.row(h).iter().position(|v| *v != 0).unwrap();
                for j in 0..mat_c.ncols() {
                    if j == k || mat_c[(h, j)] == 0 {
                        continue;
                    }

                    let chk = mat_c[(h, k)];
                    let chj = mat_c[(h, j)];
                    let alpha = match chk.signum() != chk.signum() {
                        true => chj.abs(),
                        false => -chj.abs(),
                    };
                    let beta = chk.abs();

                    for mut row in mat_c.row_iter_mut() {
                        row[j] = row[j] * beta + row[k] * alpha;
                    }
                    for mut row in mat_b.row_iter_mut() {
                        row[j] = row[j] * beta + row[k] * alpha;
                    }
                }

                mat_c = mat_c.remove_column(k);
                mat_b = mat_b.remove_column(k);
            }
        }
    }

    // phase 2
    // 1. Neg-el row
    while let Some(row) = mat_b
        .row_iter()
        .position(|row| row.iter().any(|el| *el < 0))
    {
        let (pos, neg) = PositiveNegative::partition_pos_neg(mat_c.row(row).iter());

        if !pos.is_empty() {
            for j in pos.iter() {
                for k in neg.iter() {
                    // linear combination
                    let mut combination = mat_c.column(*j).to_homogeneous();
                    let a = -mat_c[(row, *k)];
                    let b = mat_c[(row, *j)];
                    for i in 0..combination.nrows() {
                        combination[(i, 0)] = a * mat_c[(i, *j)] + b * mat_c[(i, *k)];
                    }

                    // gcd
                    let mut res = combination[(0, 0)];
                    for i in 1..combination.nrows() {
                        res = gcd(res, combination[(i, 0)]);
                    }
                    combination.apply(|el| *el /= res);

                    let cols = mat_b.ncols();
                    mat_b = mat_b.insert_column(cols, 0);
                    mat_b.set_column(mat_b.ncols() - 1, &combination)
                }
            }
        }

        mat_b = mat_b.remove_columns_at(&neg);
    }

    // sum of all solutions is invariant too
    if mat_b.ncols() > 1 {
        let cols = mat_b.ncols();
        mat_b = mat_b.insert_column(cols, 0);
        for j in 0..(mat_b.ncols() - 1) {
            for i in 0..mat_b.nrows() {
                mat_b[(i, cols)] += mat_b[(i, j)];
            }
        }
    }

    mat_b
}

fn print_matrix(mat: &DMatrix<i32>, vertices: &[String]) {
    for column in mat.column_iter() {
        let mut first = true;
        for (i, el) in column.iter().enumerate() {
            if *el == 0 {
                continue;
            }

            if !first {
                print!(" + ");
            }

            match *el {
                1 => print!("{}", vertices[i]),
                _ => print!("{}{}", el, vertices[i]),
            }

            first = false;
        }
        println!();
    }

    std::io::stdout().flush().unwrap();
}

