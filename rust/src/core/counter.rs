use std::cmp::max;

pub struct Counter {
    a: Vec<u16>,
    b: Vec<u16>,
    init: bool,
}

impl Counter {
    #[allow(unused)]
    pub fn new(max: usize) -> Counter {
        Counter {
            a: vec![0; max],
            b: vec![0; max],
            init: true,
        }
    }

    pub fn next_borrow(&mut self) -> Option<&[u16]> {
        let length = self.a.len();
        let mut c = length - 1;
        while self.a[c] == length as u16 - 1 || self.a[c] > self.b[c] {
            c = c - 1;
        }

        if c == 0 {
            return None;
        }

        self.a[c] += 1;

        for i in (c + 1)..length {
            self.a[i] = 0;
            self.b[i] = max(self.a[i - 1], self.b[i - 1]);
        }

        Some(&self.a)
    }

}

impl Iterator for Counter {
    type Item = Vec<u16>;

    fn next(&mut self) -> Option<Self::Item> {
        if self.init {
            self.init = false;
            Some(self.a.clone())
        } else {
            self.next_borrow().map(Vec::from)
        }
    }

    fn count(mut self) -> usize where Self: Sized {
        let mut counter = 1;
        while let Some(_) = self.next_borrow() {
            counter += 1;
        }
        return counter
    }
}
