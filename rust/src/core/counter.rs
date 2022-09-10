use std::collections::HashSet;

pub struct Counter {
    max: usize,
    data: Vec<usize>,
}

impl Counter {
    pub fn new(max: usize) -> Counter {
        Counter {
            max,
            data: vec![0; max]
        }
    }

    pub fn next<'a>(&'a mut self) -> Option<&'a [usize]> {
        let len = self.data.len();
        let max = self.max;
        let (i, val) = self.data
            .iter_mut()
            .enumerate()
            .rev()
            .find(|(i, value)| **value < max)
            .map(|(i, v)| (i, *v))?;

        if i > 0 && i < len - 1 {
            self.data[i + 1] = 0;
        }

        self.data[i] = val + 1;

        if has_unique_elements(self.data.iter()) {
            self.next()
        } else {
            Some(self.data.as_slice())
        }
    }
}

fn has_unique_elements<'a, T: Iterator<Item = &'a usize>>(iter: T) -> bool {
    let mut uniq = HashSet::new();
    iter.filter(|v| **v != 0).all(move |x| uniq.insert(*x))
}