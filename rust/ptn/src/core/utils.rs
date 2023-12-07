use nalgebra::DMatrix;

pub fn logical_row_add<T: Copy + PartialOrd>(matrix: &mut DMatrix<T>, a: usize, b: usize) {
    for col in 0..matrix.ncols() {
        let max = complex_max(matrix[(a, col)], matrix[(b, col)]);
        matrix[(a, col)] = max;
        matrix[(b, col)] = max;
    }
}

pub fn logical_column_add<T: Copy + PartialOrd>(matrix: &mut DMatrix<T>, a: usize, b: usize) {
    for row in 0..matrix.nrows() {
        let max = complex_max(matrix[(row, a)], matrix[(row, b)]);
        matrix[(row, a)] = max;
        matrix[(row, b)] = max;
    }
}

fn complex_max<T: Copy + PartialOrd>(a: T, b: T) -> T {
    if a > b {
        a
    } else {
        b
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_logical_row_add() {
        let mut matrix = DMatrix::from_row_slice(3, 3, &[1, 0, 0, 0, 1, 0, 0, 0, 1]);
        logical_row_add(&mut matrix, 0, 1);
        assert_eq!(
            matrix,
            DMatrix::from_row_slice(3, 3, &[1, 1, 0, 1, 1, 0, 0, 0, 1])
        );
    }

    #[test]
    fn test_logical_column_add() {
        let mut matrix = DMatrix::from_row_slice(3, 3, &[1, 0, 0, 0, 1, 0, 0, 0, 1]);
        logical_column_add(&mut matrix, 0, 1);
        assert_eq!(
            matrix,
            DMatrix::from_row_slice(3, 3, &[1, 1, 0, 1, 1, 0, 0, 0, 1])
        );
    }
}
