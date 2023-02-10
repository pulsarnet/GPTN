use std::cmp::Ordering;
use std::fmt::{Display, Formatter};
use std::ops::{Add, AddAssign, Index, IndexMut, SubAssign};
use std::slice::Iter;
use nalgebra::DMatrix;
use CVec;

mod decompose;
mod simulation;

pub use self::simulation::FiredTransition;

#[derive(PartialEq, Clone, Debug, Copy)]
enum CovType {
    DeadEnd,
    Inner,
    Boundary,
    Duplicate,
}

#[derive(Clone, Debug)]
enum MarkerValue {
    Value(i32),
    Infinity
}

impl MarkerValue {
    fn as_number(&self) -> i32 {
        match self {
            MarkerValue::Value(v) => *v,
            MarkerValue::Infinity => -1
        }
    }
}

impl PartialEq for MarkerValue {
    fn eq(&self, other: &Self) -> bool {
        match (self, other) {
            (MarkerValue::Value(a), MarkerValue::Value(b)) => { *a == *b },
            (MarkerValue::Infinity, MarkerValue::Infinity) => true,
            (MarkerValue::Value(_), MarkerValue::Infinity)
            | (MarkerValue::Infinity, MarkerValue::Value(_)) => false,
        }
    }
}

impl PartialEq<i32> for MarkerValue {
    fn eq(&self, other: &i32) -> bool {
        match self {
            MarkerValue::Value(a) => { a == other },
            MarkerValue::Infinity => false,
        }
    }
}

impl PartialOrd for MarkerValue {
    fn partial_cmp(&self, other: &Self) -> Option<Ordering> {
        match (self, other) {
            (MarkerValue::Value(a), MarkerValue::Value(b)) => { a.partial_cmp(b) },
            (MarkerValue::Infinity, MarkerValue::Infinity) => Some(Ordering::Less),
            (MarkerValue::Value(_), MarkerValue::Infinity) => Some(Ordering::Less),
            (MarkerValue::Infinity, MarkerValue::Value(_)) => Some(Ordering::Greater),
        }
    }
}

impl PartialOrd<i32> for MarkerValue {
    fn partial_cmp(&self, other: &i32) -> Option<Ordering> {
        match self {
            MarkerValue::Value(a) => { a.partial_cmp(other) }
            MarkerValue::Infinity => Some(Ordering::Greater)
        }
    }
}

impl Add for MarkerValue {
    type Output = Self;

    fn add(self, rhs: Self) -> Self::Output {
        match (self, rhs) {
            (MarkerValue::Value(a), MarkerValue::Value(b)) => { MarkerValue::Value(a + b) },
            (MarkerValue::Infinity, MarkerValue::Infinity)
            | (MarkerValue::Value(_), MarkerValue::Infinity)
            | (MarkerValue::Infinity, MarkerValue::Value(_)) => { MarkerValue::Infinity },
        }
    }
}

impl Display for MarkerValue {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        match self {
            MarkerValue::Value(a) => { write!(f, "{}", a)}
            MarkerValue::Infinity => { write!(f, "w") }
        }
    }
}

impl SubAssign<i32> for MarkerValue {
    fn sub_assign(&mut self, rhs: i32) {
        if let MarkerValue::Value(a) = self {
            *a -= rhs;
        }
    }
}

impl AddAssign<i32> for MarkerValue {
    fn add_assign(&mut self, rhs: i32) {
        if let MarkerValue::Value(a) = self {
            *a += rhs;
        }
    }
}

impl num_traits::Zero for MarkerValue {
    fn zero() -> Self {
        MarkerValue::Value(0)
    }

    fn is_zero(&self) -> bool {
        matches!(self, MarkerValue::Value(0))
    }
}

#[derive(Clone, Debug)]
pub struct Marking {
    type_: CovType,
    data: DMatrix<MarkerValue>,

    prev: Option<(usize, usize)>,
    next: Vec<(usize, usize)>,
}

impl From<DMatrix<i32>> for Marking {
    fn from(data: DMatrix<i32>) -> Self {
        Marking {
            type_: CovType::Boundary,
            data: {
                let mut marking = DMatrix::<MarkerValue>::zeros(data.nrows(), data.ncols());
                for row in 0..data.nrows() {
                    for col in 0..data.ncols() {
                        marking.row_mut(row)[col] = MarkerValue::Value(data.row(row)[col]);
                    }
                }
                marking
            },
            prev: None,
            next: vec![],
        }
    }
}

pub struct Reachability {
    input: DMatrix<i32>,
    output: DMatrix<i32>,

    markings: ReachabilityTree,
}

impl Reachability {
    pub fn new(input: DMatrix<i32>, output: DMatrix<i32>, marking: DMatrix<i32>) -> Self {
        Reachability {
            input,
            output,
            markings: ReachabilityTree::new(Marking::from(marking)),
        }
    }

    pub fn selector(&self, marking: &Marking) -> Vec<(usize, Marking)> {
        let mut markings = vec![];

        'main: for transition in 0..self.input.ncols() {
            let input_col = self.input.column(transition);
            let output_col = self.output.column(transition);

            for position in 0..self.input.nrows() {
                if marking.data.row(0)[position] < input_col[position] {
                    continue 'main
                }
            }

            let mut new_marking = marking.clone();
            for position in 0..self.input.nrows() {
                new_marking.data.row_mut(0)[position] -= input_col[position];
                new_marking.data.row_mut(0)[position] += output_col[position];
            }

            markings.push((transition, new_marking));
        }

        markings
    }

    pub fn compute(&mut self) -> ReachabilityTree {
        loop {
            // Когда все вершины дерева – терминальные, дублирующие или внутренние, алгоритм останавливается
            if !self.markings.has_boundary() {
                break
            }

            for marking in 0..self.markings.count() {
                let marking_type = self.markings[marking].type_;
                if marking_type != CovType::Boundary {
                    continue
                }

                // Если в дереве имеется другая вершина Y, не являющаяся
                // граничной, и с ней связана та же маркировка, M(X) = M(Y)
                // то вершина X дублирующая
                if let Some(_) = self.markings.iter()
                    .filter(|mark| mark.type_ != CovType::Boundary)
                    .find(|mark| {
                        mark.data == self.markings[marking].data
                    })
                {
                    self.markings[marking].type_ = CovType::Duplicate;
                    continue
                }

                // Если для маркировки X не один из переходов не разрешен
                // то вершина X терминальная
                let mut selector = self.selector(&self.markings[marking]);
                if selector.is_empty() {
                    self.markings[marking].type_ = CovType::DeadEnd;
                    continue
                }

                // Для всякого перехода t, разрешенного в M(X), создать новую вершину Z дерева достижимости
                for (transition, select) in selector.iter_mut() {
                    // Если на пути от корневой вершины к X существует вершина Y с
                    // M(Y) < M(R), где M(R) удовлетворяет M(X)->(transition)->M(R),
                    // и M(Y)[Pj] < M(X)[Pj], то вершина M(Z)[Pj] = W
                    let mut select_clone = select.clone();
                    let mut index = self.markings[marking].prev;
                    while let Some((t, i)) = index {
                        index = self.markings[i].prev;

                        // if t != *transition {
                        //     continue;
                        // }
                        if self.markings[i].data.row(0) >= select_clone.data.row(0) {
                            continue;
                        }

                        for position in 0..self.input.nrows() {
                            if self.markings[i].data[(0, position)] < select_clone.data[(0, position)] {
                                select.data[(0, position)] = MarkerValue::Infinity;
                            }
                        }
                    }
                }

                // Дуга, помеченная tj,
                // направлена от вершины х к вершине z.
                // Вершина х переопределяется как внутренняя, вершина z становится граничной.
                for (transition, mut select) in selector.into_iter() {
                    select.type_ = CovType::Boundary;
                    select.prev = Some((transition, marking));
                    self.markings.append(select);

                    let index = self.markings.count() - 1;
                    self.markings[marking].next.push((transition, index));
                    self.markings[marking].type_ = CovType::Inner;
                }
            }
        }

        let mut tree = ReachabilityTree::new(self.markings.markings[0].clone());
        std::mem::swap(&mut tree, &mut self.markings);
        tree
    }
}


pub struct ReachabilityTree {
    markings: Vec<Marking>
}

impl ReachabilityTree {
    fn new(init: Marking) -> Self {
        ReachabilityTree {
            markings: vec![init]
        }
    }

    fn has_boundary(&self) -> bool {
        self.markings.iter().find(|mark| mark.type_ == CovType::Boundary).is_some()
    }

    fn count(&self) -> usize {
        self.markings.len()
    }

    fn append(&mut self, value: Marking) {
        self.markings.push(value)
    }

    fn iter(&self) -> Iter<'_, Marking> {
        self.markings.iter()
    }
}

impl Index<usize> for ReachabilityTree {
    type Output = Marking;

    fn index(&self, index: usize) -> &Self::Output {
        &self.markings[index]
    }
}

impl IndexMut<usize> for ReachabilityTree {
    fn index_mut(&mut self, index: usize) -> &mut Self::Output {
        &mut self.markings[index]
    }
}

#[no_mangle]
extern "C" fn reachability_marking(this: &ReachabilityTree, vec: &mut CVec<*const Marking>) {
    let result = this.markings.iter()
        .map(|marking| marking as *const _)
        .collect::<Vec<_>>();

    unsafe { core::ptr::write_unaligned(vec, CVec::from(result)) };
}

#[no_mangle]
unsafe extern "C" fn reachability_drop(this: *mut ReachabilityTree) {
    let _ = Box::from_raw(this);
}

#[no_mangle]
extern "C" fn marking_values(this: *const Marking, vec: &mut CVec<i32>) {
    let this = unsafe { &*this };
    *vec = this.data.row(0).iter()
        .map(MarkerValue::as_number)
        .collect::<Vec<_>>()
        .into();
}

#[no_mangle]
extern "C" fn marking_previous(this: &Marking) -> i32 {
    this.prev.map(|v| v.1 as i32).unwrap_or(-1)
}

#[no_mangle]
extern "C" fn marking_transition(this: &Marking) -> i32 {
    this.prev.map(|v| v.0 as i32).unwrap_or(-1)
}


#[cfg(test)]
mod tests {
    use std::hash::Hash;
    use nalgebra::DMatrix;
    use modules::reachability::Reachability;

    #[test]
    pub fn test_cov() {
        let mut input = DMatrix::<i32>::zeros(4, 3);
        let mut output = DMatrix::<i32>::zeros(4, 3);
        let mut markers = DMatrix::<i32>::zeros(1, 4);

        // p1
        input.row_mut(0)[0] = 1;
        input.row_mut(0)[1] = 1;
        output.row_mut(0)[0] = 1;

        // p2
        input.row_mut(1)[2] = 1;
        output.row_mut(1)[0] = 1;

        // p3
        input.row_mut(2)[2] = 1;
        output.row_mut(2)[2] = 1;
        output.row_mut(2)[1] = 1;

        // p4
        output.row_mut(3)[2] = 1;

        markers.row_mut(0)[0] = 1;

        println!("{}", input);
        println!("{}", output);

        let mut cov = Reachability::new(input, output, markers);
        let tree = cov.compute();

        let mut current = vec![0];
        let mut level = 0;
        loop {
            if current.is_empty() {
                break
            }

            for i in 0..current.len() {
                let prev = match cov.markings[current[i]].prev {
                    Some((_, m)) => &cov.markings[m].data,
                    None => &cov.markings[0].data
                };
                print!("{level}: {} => {} => {}",
                    prev,
                    cov.markings[current[i]].prev.unwrap_or((0, 0)).0,
                    cov.markings[current[i]].data);
            }
            println!();

            current = current.clone().iter()
                .map(|i| &cov.markings[*i])
                .map(|m| &m.next)
                .flatten()
                .map(|(_, i)| *i)
                .collect::<Vec<_>>();

            level += 1;
        }
    }
}