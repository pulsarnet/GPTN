use nalgebra::DMatrix;

pub trait MatrixExt<T> {
    fn logical_row_add(&mut self, a: usize, b: usize);
    
    fn logical_column_add(&mut self, a: usize, b: usize);
}

impl<T: Copy + PartialOrd> MatrixExt<T> for DMatrix<T> {
    fn logical_row_add(&mut self, a: usize, b: usize) {
        for col in 0..self.ncols() {
            let max = complex_max(self[(a, col)], self[(b, col)]);
            self[(a, col)] = max;
            self[(b, col)] = max;
        }
    }

    fn logical_column_add(&mut self, a: usize, b: usize) {
        for row in 0..self.nrows() {
            let max = complex_max(self[(row, a)], self[(row, b)]);
            self[(row, a)] = max;
            self[(row, b)] = max;
        }
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
    fn logical_row_add() {
        let mut matrix = DMatrix::from_row_slice(3, 3, &[1, 0, 0, 0, 1, 0, 0, 0, 1]);
        matrix.logical_row_add( 0, 1);
        assert_eq!(
            matrix,
            DMatrix::from_row_slice(3, 3, &[1, 1, 0, 1, 1, 0, 0, 0, 1])
        );
    }

    #[test]
    fn logical_column_add() {
        let mut matrix = DMatrix::from_row_slice(3, 3, &[1, 0, 0, 0, 1, 0, 0, 0, 1]);
        matrix.logical_column_add(0, 1);
        assert_eq!(
            matrix,
            DMatrix::from_row_slice(3, 3, &[1, 1, 0, 1, 1, 0, 0, 0, 1])
        );
    }
    
    #[test]
    fn find_complex_max() {
        assert_eq!(complex_max(0, 1), 1);
        assert_eq!(complex_max(1, 0), 1);
        assert_eq!(complex_max(2, 2), 2);
    }
}
