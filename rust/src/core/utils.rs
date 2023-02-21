use nalgebra::DMatrix;
use num_traits::Zero;

pub fn logical_row_add<T: Copy + Zero + PartialEq>(matrix: &mut DMatrix<T>, a: usize, b: usize) {
    for col in 0..matrix.ncols() {
        let value = matrix[(b, col)];
        if value != T::zero() {
            matrix[(a, col)] = value;
        }
    }
}

pub fn logical_column_add<T: Copy + Zero + PartialEq>(matrix: &mut DMatrix<T>, a: usize, b: usize) {
    for row in 0..matrix.nrows() {
        let value = matrix[(row, b)];
        if value != T::zero() {
            matrix[(row, a)] = value;
        }
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
            DMatrix::from_row_slice(3, 3, &[1, 1, 0, 0, 1, 0, 0, 0, 1])
        );
    }

    #[test]
    fn test_logical_column_add() {
        let mut matrix = DMatrix::from_row_slice(3, 3, &[1, 0, 0, 0, 1, 0, 0, 0, 1]);
        logical_column_add(&mut matrix, 0, 1);
        assert_eq!(
            matrix,
            DMatrix::from_row_slice(3, 3, &[1, 0, 0, 1, 1, 0, 0, 0, 1])
        );
    }
}
