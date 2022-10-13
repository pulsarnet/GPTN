use nalgebra::{DMatrix, Dynamic, MatrixSlice, U1};
use ::{PetriNet, Vertex};
use ::{DecomposeContextBuilder, PetriNetVec};

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

        let positions_count = net.positions.len();
        let transitions_count = net.transitions.len();

        if positions_count > 2 *  transitions_count {
            return Err(())
        }

        // Добавим эквивалентные позиции
        let mut need = 2 * transitions_count - positions_count;
        if need > 0 {
            // Получим внутренние позиции
            let filter = net.positions.iter().filter(|(index, position)| {
                net.connections.iter().find(|connection| connection.first().eq(index)).is_some()
                && net.connections.iter().find(|connection| connection.second().eq(index)).is_some()
            })
                .take(need)
                .map(|(index, position)| (index.clone(), position.clone()))
                .collect::<Vec<_>>();

            if filter.len() < need {
                return Err(());
            }

            let mut iter = filter.iter();

            while need > 0 {
                let Some((index, position)) = iter.next() else { break };
                let position_index = net.next_position_index();

                let new_position = Vertex::position(position_index);
                let new_index = new_position.index();

                net.insert_position(new_position);
                let connections = net.connections.iter().filter(|connection| {
                    connection.first().eq(index) || connection.second().eq(index)
                })
                    .cloned()
                    .collect::<Vec<_>>();

                connections.into_iter().for_each(|connection| {
                    if connection.first().eq(index) {
                        net.connect(new_index, connection.second());
                    } else {
                        net.connect(connection.first(), new_index);
                    }
                });

                need -= 1;
            }

            if need > 0 {
                return Err(());
            }
        }

        // Создадим матрицы D(I) и D(O)
        let (d_input, d_output) = net.incidence_matrix();

        // Получим общую матрицу инцидентности D
        let d_matrix = d_output.matrix - d_input.matrix;

        // Создадим примитивную систему
        let Some(primitive_matrix) = simplify_matrix(d_matrix) else { return Err(()) };
        let mut primitive_net = PetriNet::new();
        net.positions.iter().for_each(|(_, vertex)| {
            primitive_net.insert_position(vertex.clone());
        });

        net.transitions.iter().for_each(|(_, vertex)| {
            primitive_net.insert_transition(vertex.clone());
        });

        for row_index in 0..primitive_matrix.nrows() {
            for column_index in 0..primitive_matrix.ncols() {
                let value = primitive_matrix.row(row_index)[column_index];
                if value > 0 {
                    primitive_net.connect(
                        net.transitions[column_index].index(),
                        net.positions[row_index].index(),
                    )
                } else if value < 0 {
                    primitive_net.connect(
                        net.transitions[column_index].index(),
                        net.positions[row_index].index(),
                    )
                }
            }
        }

        // Создадим матрицы примитивной системы DP(I) и DP(O)
        let (primitive_input, primitive_output) = primitive_net.incidence_matrix();

        // Вычислим тензоры C(I) и C(O)
        let (c_input, c_output) = DecomposeContextBuilder::calculate_c_matrix2(
            &PetriNetVec(vec![primitive_net.clone()])
        );

        // Вычислим тензоры E(I) и E(O)
        let Some(e_input) = c_input.inner.map(|i| i as f64).try_inverse()
            .map(|matrix| matrix.map(|i| i as i32))
            else { return Err(()); };

        let Some(e_output) = c_output.inner.map(|i| i as f64).try_inverse()
            .map(|matrix| matrix.map(|i| i as i32))
            else { return Err(()); };

        Ok(Self {
            net,
            primitive_net,
            input_tensor: c_input.inner,
            output_tensor: c_output.inner,
            inverse_input_tensor: e_input,
            inverse_output_tensor: e_output
        })
    }
}

// Функция выбирает строки, которые удовлетворяют итоговому условию:
// Каждая строка всегда должна иметь ТОЛЬКО одно значение из массива [-1, 1] остальные 0
fn deny_rows(matrix: &DMatrix<i32>) -> Vec<usize> {
    matrix.row_iter().enumerate()
        .filter(|(_, row)| {
            row.iter().filter(|element| **element != 0).count() == 1
        })
        .map(|(index, _)| index)
        .collect::<Vec<_>>()
}

// Функция проверяет, что в колонке есть ХОТЯ БЫ ОДНА -1 и ХОТЯ БЫ ОДНА 1
fn column_condition(slice: &MatrixSlice<i32, Dynamic, U1>) -> bool {
    slice.iter().filter(|element| **element == -1).count() > 0
        && slice.iter().filter(|element| **element == 1).count() > 0
}

// Функция выбирает колонки, которые удовлетворяют итоговому условию:
// каждый столбец должен содержать только ОДНУ -1 И ОДНУ 1
fn deny_cols(matrix: &DMatrix<i32>) -> Vec<usize> {
    matrix.column_iter().enumerate()
        .filter(|(_, column)| {
            column.iter().filter(|element| **element == -1).count() == 1
                && column.iter().filter(|element| **element == 1).count() == 1
        })
        .map(|(index, _)| index)
        .collect::<Vec<_>>()
}

fn simplify_matrix(mut matrix: DMatrix<i32>) -> Option<DMatrix<i32>> {

    let mut deny_cells = vec![];

    'main: loop {

        // Получаем строки и колонки, которые удовлетворяют итоговому условию
        // И запрещаем их обработку
        let deny_rows = deny_rows(&matrix);
        let deny_cols = deny_cols(&matrix);

        // Если все колонки и строки удовлетворяют итоговому условию, то решение найдено
        if deny_rows.len() == 8 || deny_cols.len() == 4 {
            break;
        }

        for row_index in 0..matrix.nrows() {
            if deny_rows.contains(&row_index) {
                // Строка запрещена для обработки:
                // 1) Если это последняя, то нет решения
                // 2) Иначе пропускаем
                if row_index == matrix.nrows() - 1 {
                    return None
                }

                continue
            }

            for column_index in 0..matrix.ncols() {
                if deny_cols.contains(&column_index) {
                    // Колонка запрещена для обработки, пропускаем
                    continue
                }

                // Проверим, что ячейка не заблокирована (посещена)
                if deny_cells.contains(&(row_index, column_index)) {
                    continue;
                }

                // Проверим, что значение не равно нулю
                if matrix.row(row_index)[column_index] == 0 {
                    continue;
                }

                // Заменим значение на 0
                let value = matrix.row(row_index)[column_index];
                matrix.row_mut(row_index)[column_index] = 0;

                // Проверим, можем ли мы заменить значение.
                // В колонке должно остаться ХОТЯ БЫ ОДНА 1 и ОДНА -1
                if !column_condition(&matrix.column(column_index)) {
                    // Условие не выполнено
                    // Возвращаем старое значение
                    matrix.row_mut(row_index)[column_index] = value;

                    // Отметим ячейку как посещенную, чтобы не обрабатывать её повторно
                    deny_cells.push((row_index, column_index));

                    // Продолжим обрабатывать ячейки
                    continue
                }

                // Если это последняя ячейка, то можно сказать, что решение найдено ????
                if row_index == matrix.nrows() - 1 && column_index == matrix.ncols() - 1 {
                    break 'main
                }

                // В случае изменения значения надо начать цикл заново
                continue 'main
            }
        }
    }

    Some(matrix)
}