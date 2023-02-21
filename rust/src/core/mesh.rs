use nalgebra::DMatrix;

// Set Partition Mesh https://ajc.maths.uq.edu.au/pdf/65/ajc_v65_p152.pdf
#[derive(Debug)]
pub struct SetPartitionMesh {
    positions: DMatrix<usize>,
    transitions: DMatrix<usize>,
    max: usize,
}

impl SetPartitionMesh {
    pub fn new(positions: usize, transitions: usize) -> SetPartitionMesh {
        let position_mesh = make_mesh(positions);
        let transition_mesh = make_mesh(transitions);
        let max = position_mesh[(0, 0)] * transition_mesh[(0, 0)];

        SetPartitionMesh {
            positions: position_mesh,
            transitions: transition_mesh,
            max,
        }
    }

    pub fn max(&self) -> usize {
        self.max
    }

    pub fn len(&self) -> usize {
        self.transitions.nrows() + self.positions.nrows()
    }

    // Traversal of a Set Partition Mesh
    pub fn get_partition(&self, index: usize) -> Vec<u16> {
        if index >= self.max {
            panic!("Index out of bounds");
        }

        // rev index
        //let index = self.max - index - 1;

        // first is positions, second is transitions
        let position_index = index / self.transitions[(0, 0)];
        let transition_index = index % self.transitions[(0, 0)];

        let mut sets = get_partition_by_index(&self.transitions, transition_index);
        sets.extend(get_partition_by_index(&self.positions, position_index));

        sets
    }
}

fn make_mesh(size: usize) -> DMatrix<usize> {
    let mut mesh = DMatrix::zeros(size, size);
    mesh.row_mut(mesh.nrows() - 1).fill(1);

    // Equation from https://ajc.maths.uq.edu.au/pdf/65/ajc_v65_p152.pdf page 163(12)
    for i in (0..(size - 1)).rev() {
        for j in 0..=i {
            mesh[(i, j)] = (j + 1) * mesh[(i + 1, j)] + mesh[(i + 1, j + 1)];
        }
    }

    mesh
}

fn get_partition_by_index(mesh: &DMatrix<usize>, mut index: usize) -> Vec<u16> {
    let n = mesh.nrows();
    let mut partition = vec![0; n];
    let (mut i, mut j) = (1, 0);

    while i < n {
        let factor = std::cmp::min(index / mesh[(i, j)], j + 1);
        partition[i] = factor as u16;
        index -= factor * mesh[(i, j)];
        if factor == (j + 1) {
            j += 1;
        }
        i += 1;
    }

    partition
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_mesh() {
        let mesh = SetPartitionMesh::new(4, 4);
        assert_eq!(mesh.max, 225);
        assert_eq!(
            mesh.positions,
            DMatrix::from_row_slice(4, 4, &[15, 0, 0, 0, 5, 10, 0, 0, 2, 3, 4, 0, 1, 1, 1, 1,])
        );
        assert_eq!(
            mesh.transitions,
            DMatrix::from_row_slice(4, 4, &[15, 0, 0, 0, 5, 10, 0, 0, 2, 3, 4, 0, 1, 1, 1, 1,])
        );

        assert_eq!(mesh.get_partition(0), vec![0, 0, 0, 0, 0, 0, 0, 0]);
        assert_eq!(mesh.get_partition(1), vec![0, 0, 0, 1, 0, 0, 0, 0]);
        assert_eq!(mesh.get_partition(2), vec![0, 0, 1, 0, 0, 0, 0, 0]);
        assert_eq!(mesh.get_partition(3), vec![0, 0, 1, 1, 0, 0, 0, 0]);
        assert_eq!(mesh.get_partition(4), vec![0, 0, 1, 2, 0, 0, 0, 0]);
        assert_eq!(mesh.get_partition(5), vec![0, 1, 0, 0, 0, 0, 0, 0]);
        assert_eq!(mesh.get_partition(6), vec![0, 1, 0, 1, 0, 0, 0, 0]);
        assert_eq!(mesh.get_partition(7), vec![0, 1, 0, 2, 0, 0, 0, 0]);
        assert_eq!(mesh.get_partition(8), vec![0, 1, 1, 0, 0, 0, 0, 0]);
        assert_eq!(mesh.get_partition(9), vec![0, 1, 1, 1, 0, 0, 0, 0]);
        assert_eq!(mesh.get_partition(10), vec![0, 1, 1, 2, 0, 0, 0, 0]);
        assert_eq!(mesh.get_partition(11), vec![0, 1, 2, 0, 0, 0, 0, 0]);
        assert_eq!(mesh.get_partition(12), vec![0, 1, 2, 1, 0, 0, 0, 0]);
        assert_eq!(mesh.get_partition(13), vec![0, 1, 2, 2, 0, 0, 0, 0]);
        assert_eq!(mesh.get_partition(14), vec![0, 1, 2, 3, 0, 0, 0, 0]);
    }
}
