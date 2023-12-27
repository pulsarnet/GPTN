use nalgebra::{DMatrix, SimdValue};
use net::vertex::VertexIndex;
use net::PetriNet;
use std::cmp::Ordering;
use std::fmt::{Display, Formatter};
use std::io::Write;
use std::ops::{Add, AddAssign, Index, IndexMut, SubAssign};
use std::slice::Iter;

#[allow(unused)]
mod decompose;
#[allow(unused)]
pub mod simulation;

#[derive(PartialEq, Clone, Debug, Copy)]
#[repr(u8)]
pub enum CovType {
    DeadEnd = 0,
    Inner = 1,
    Boundary = 2,
    Duplicate = 3,
}

#[derive(Clone, Copy, Debug)]
pub enum MarkerValue {
    Value(i32),
    Infinity,
}

impl MarkerValue {
    /// -1 - inf
    /// 0..=i32::MAX - number
    pub fn as_number(&self) -> i32 {
        match self {
            MarkerValue::Value(v) => *v,
            MarkerValue::Infinity => -1,
        }
    }
}

impl Ord for MarkerValue {
    fn cmp(&self, other: &Self) -> Ordering {
        match (self, other) {
            (MarkerValue::Value(a), MarkerValue::Value(b)) => a.cmp(b),
            (MarkerValue::Infinity, MarkerValue::Infinity) => Ordering::Less,
            (MarkerValue::Value(_), MarkerValue::Infinity) => Ordering::Less,
            (MarkerValue::Infinity, MarkerValue::Value(_)) => Ordering::Greater,
        }
    }
}

impl Eq for MarkerValue {}

impl PartialEq for MarkerValue {
    fn eq(&self, other: &Self) -> bool {
        match (self, other) {
            (MarkerValue::Value(a), MarkerValue::Value(b)) => *a == *b,
            (MarkerValue::Infinity, MarkerValue::Infinity) => true,
            (MarkerValue::Value(_), MarkerValue::Infinity)
            | (MarkerValue::Infinity, MarkerValue::Value(_)) => false,
        }
    }
}

impl PartialEq<i32> for MarkerValue {
    fn eq(&self, other: &i32) -> bool {
        match self {
            MarkerValue::Value(a) => a == other,
            MarkerValue::Infinity => false,
        }
    }
}

impl PartialOrd for MarkerValue {
    fn partial_cmp(&self, other: &Self) -> Option<Ordering> {
        match (self, other) {
            (MarkerValue::Value(a), MarkerValue::Value(b)) => a.partial_cmp(b),
            (MarkerValue::Infinity, MarkerValue::Infinity) => Some(Ordering::Less),
            (MarkerValue::Value(_), MarkerValue::Infinity) => Some(Ordering::Less),
            (MarkerValue::Infinity, MarkerValue::Value(_)) => Some(Ordering::Greater),
        }
    }
}

impl PartialOrd<i32> for MarkerValue {
    fn partial_cmp(&self, other: &i32) -> Option<Ordering> {
        match self {
            MarkerValue::Value(a) => a.partial_cmp(other),
            MarkerValue::Infinity => Some(Ordering::Greater),
        }
    }
}


impl Add for MarkerValue {
    type Output = Self;

    fn add(self, rhs: Self) -> Self::Output {
        match (self, rhs) {
            (MarkerValue::Value(a), MarkerValue::Value(b)) => MarkerValue::Value(a + b),
            (MarkerValue::Infinity, MarkerValue::Infinity)
            | (MarkerValue::Value(_), MarkerValue::Infinity)
            | (MarkerValue::Infinity, MarkerValue::Value(_)) => MarkerValue::Infinity,
        }
    }
}

impl Display for MarkerValue {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        match self {
            MarkerValue::Value(a) => {
                write!(f, "{}", a)
            }
            MarkerValue::Infinity => {
                write!(f, "w")
            }
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

    prev: Option<(VertexIndex, usize)>,
    next: Vec<(VertexIndex, usize)>,
}

impl Marking {
    pub fn data(&self) -> &DMatrix<MarkerValue> {
        &self.data
    }
    
    pub fn r#type(&self) -> CovType {
        self.type_
    }

    pub fn prev(&self) -> Option<&(VertexIndex, usize)> {
        self.prev.as_ref()
    }
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
    positions: Vec<VertexIndex>,
    transitions: Vec<VertexIndex>,
    input: DMatrix<i32>,
    output: DMatrix<i32>,

    marking: Marking,
}

impl Reachability {
    pub fn new(net: &PetriNet) -> Self {
        let (input, output) = net.incidence_matrix();
        let marking = net.marking();

        Reachability {
            positions: net.positions().keys().cloned().collect(),
            transitions: net.transitions().keys().cloned().collect(),
            input,
            output,
            marking: Marking::from(marking),
        }
    }

    /// Получить возможные маркировки из текущей
    pub fn selector(&self, marking: &Marking) -> Vec<(VertexIndex, Marking)> {
        let mut markings = vec![];

        'main: for transition in 0..self.input.ncols() {
            let input_col = self.input.column(transition);
            let output_col = self.output.column(transition);

            for position in 0..self.input.nrows() {
                if marking.data.row(0)[position] < input_col[position] {
                    continue 'main;
                }
            }

            let mut new_marking = marking.clone();
            for position in 0..self.input.nrows() {
                new_marking.data.row_mut(0)[position] -= input_col[position];
                new_marking.data.row_mut(0)[position] += output_col[position];
            }

            let transition = self.transitions[transition];
            markings.push((transition, new_marking));
        }

        markings
    }

    /// Возвращает дерево достижимых разметок
    pub fn compute(&self) -> Option<ReachabilityTree> {
        let mut tree = ReachabilityTree::new(self.marking.clone(), self.positions.clone());
        loop {
            // Когда все вершины дерева – терминальные, дублирующие или внутренние, алгоритм останавливается
            if tree.count() > 5000 {
                return None;
            }
            if !tree.has_boundary() {
                break;
            }

            for marking in 0..tree.count() {
                let marking_type = tree[marking].type_;
                if marking_type != CovType::Boundary {
                    continue;
                }

                // Если в дереве имеется другая вершина Y, не являющаяся
                // граничной, и с ней связана та же маркировка, M(X) = M(Y)
                // то вершина X дублирующая
                if let Some(_) = tree
                    .iter()
                    .filter(|mark| mark.type_ != CovType::Boundary)
                    .find(|mark| mark.data == tree[marking].data)
                {
                    tree[marking].type_ = CovType::Duplicate;
                    continue;
                }

                // Если для маркировки X не один из переходов не разрешен
                // то вершина X терминальная
                let mut selector = self.selector(&tree[marking]);
                if selector.is_empty() {
                    tree[marking].type_ = CovType::DeadEnd;
                    continue;
                }

                // Для всякого перехода t, разрешенного в M(X), создать новую вершину Z дерева достижимости
                for (transition, select) in selector.iter_mut() {
                    // On the path from the root to M if there exists a marking M"
                    // such that M'(p)≥M"(p) for each place p and M'M", i.e., M" is coverable,
                    // then replace M'(p) by o for each p such that M'(p)>M"(p).
                    let mut index = Some((transition.clone(), marking));
                    while let Some((_, i)) = index {
                        if i == 0 {
                            // TODO: если это root дерева то его не учитываем
                            break;
                        }
                        index = tree[i].prev;
                        if select.data.row(0) >= tree[i].data.row(0) && select.data.row(0) != tree[i].data.row(0) {
                            for position in 0..self.input.nrows() {
                                if select.data[(0, position)] > tree[i].data[(0, position)] {
                                    select.data[(0, position)] = MarkerValue::Infinity;

                                    // todo

                                }
                            }
                        }
                    }
                }

                // Дуга, помеченная tj,
                // направлена от вершины х к вершине z.
                // Вершина х переопределяется как внутренняя, вершина z становится граничной.
                for (transition, mut select) in selector.into_iter() {
                    let contains_w = select.data.iter().find(|w| **w == MarkerValue::Infinity).is_some();
                    if contains_w {
                        select.type_ = CovType::DeadEnd; // todo тупикова, дублирующая, омега (бесконечная)
                    } else {
                        select.type_ = CovType::Boundary;
                    }
                    select.prev = Some((transition, marking));
                    tree.append(select);

                    let index = tree.count() - 1;
                    tree[marking].next.push((transition, index));
                    tree[marking].type_ = CovType::Inner;
                }
            }
        }
        Some(tree)
    }
}

pub struct ReachabilityTree {
    positions: Vec<VertexIndex>,
    markings: Vec<Marking>,
}

impl ReachabilityTree {
    fn new(init: Marking, positions: Vec<VertexIndex>) -> Self {
        ReachabilityTree {
            positions,
            markings: vec![init],
        }
    }
    
    pub fn positions(&self) -> &[VertexIndex] {
        &self.positions
    }

    pub fn markings(&self) -> &[Marking] {
        &self.markings
    }

    fn get_boundary(&self) -> Option<&Marking> {
        self.markings
            .iter()
            .find(|mark| mark.type_ == CovType::Boundary)
    }

    fn has_boundary(&self) -> bool {
        self.get_boundary().is_some()
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

#[cfg(test)]
mod tests {
    use modules::reachability::Reachability;
    use net::PetriNet;

    #[test]
    pub fn test_cov() {
        let mut net = PetriNet::new();
        let p1 = net.add_position(1).index();
        let p2 = net.add_position(2).index();
        let p3 = net.add_position(3).index();
        let p4 = net.add_position(4).index();

        let t1 = net.add_transition(1).index();
        let t2 = net.add_transition(2).index();
        let t3 = net.add_transition(3).index();

        // p1
        net.connect(p1, t1);
        net.connect(p1, t2);
        net.connect(t1, p1);

        // p2
        net.connect(p2, t3);
        net.connect(t1, p2);

        // p3
        net.connect(p3, t3);
        net.connect(t3, p3);
        net.connect(t2, p3);

        // p4
        net.connect(t3, p4);

        // marking
        net.positions_mut().get_mut(&p1).unwrap().add_marker();

        let cov = Reachability::new(&net);
        let tree = cov.compute();

        let mut current = vec![0];
        let mut level = 0;
        loop {
            if current.is_empty() {
                break;
            }

            for i in 0..current.len() {
                let prev = match tree[current[i]].prev {
                    Some((_, m)) => &tree[m].data,
                    None => &tree[0].data,
                };
                print!(
                    "{level}: {} => T{} => {}",
                    prev,
                    tree[current[i]].prev.map(|(v, _)| v.id).unwrap_or(9999),
                    tree[current[i]].data
                );
            }
            println!();

            current = current
                .clone()
                .iter()
                .map(|i| &tree[*i])
                .map(|m| &m.next)
                .flatten()
                .map(|(_, i)| *i)
                .collect::<Vec<_>>();

            level += 1;
        }
    }
}
