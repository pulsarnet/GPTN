
mod connection;
mod vertex;

use std::collections::{HashMap, HashSet};
pub use net::connection::Connection;
pub use net::vertex::Vertex;
use crate::core::Unique;

#[derive(Default, Debug)]
pub struct PetriNet {
    elements: Vec<Vertex>,
    connections: Vec<Connection>,
}

impl PetriNet {
    pub fn new() -> Self {
        PetriNet::default()
    }

    pub fn get_position(&self, index: u64) -> Option<&Vertex> {
        self.elements
            .iter()
            .filter(|p| p.is_position())
            .find(|p| (*p).eq(&Vertex::Position(index)))
    }

    pub fn get_transition(&self, index: u64) -> Option<&Vertex> {
        self.elements
            .iter()
            .filter(|p| p.is_transition())
            .find(|p| (*p).eq(&Vertex::Transition(index)))
    }

    pub fn add(&mut self, element: Vertex) -> Vertex {
        if let Some(_) = self.elements.iter().find(|e| (*e).eq(&element)) {
            return element
        }
        self.elements.push(element.clone());
        element
    }

    pub fn connect(&mut self, a: &Vertex, b: &Vertex) {
        if !self.elements.contains(a) || !self.elements.contains(b) {
            return
        }

        self.connections.push(Connection::new(a.clone(), b.clone()));
    }

    pub fn get_loop(&self) -> Option<Vec<Vertex>> {

        let mut loops = Vec::<Vec<Vertex>>::new();

        let mut positions = Vec::new();
        let mut transitions = Vec::new();
        for element in self.elements.iter().cloned() {
            match element {
                Vertex::Transition(_) => transitions.push(element),
                Vertex::Position(_) => positions.push(element)
            }
        }

        let indexes = transitions
            .iter()
            .chain(positions.iter())
            .cloned()
            .enumerate()
            .fold(HashMap::new(), |mut acc, (index, element)| {
                acc.insert(element, index);
                acc
            });

        let mut pre = HashMap::<Vertex, HashSet<Vertex>>::new();
        let mut post = HashMap::<Vertex, HashSet<Vertex>>::new();
        for connection in self.connections.iter() {
            pre
                .entry(connection.second().clone())
                .or_insert_with(HashSet::new)
                .insert(connection.first().clone());

            post
                .entry(connection.first().clone())
                .or_insert_with(HashSet::new)
                .insert(connection.second().clone());
        }

        let mut b = Vec::new();
        b.resize(indexes.len(), Default::default());
        for index in indexes.iter() {
            b[*index.1] = index.0.clone();
        }


        let mut a_static = nalgebra::DMatrix::<u8>::zeros(b.len(), b.len());
        for (element, children) in pre.iter() {
            let Some(element_index) = indexes.get(element) else { continue };
            for child in children.iter() {
                let Some(child_index) = indexes.get(child) else { continue };
                match element {
                    Vertex::Position(_) => a_static.row_mut(*element_index)[*child_index] = 1,
                    Vertex::Transition(_) => a_static.column_mut(*child_index)[*element_index] = 1
                }
            }
        }

        let mut ad = a_static.clone();
        for i in 2..=b.len() {
            ad *= a_static.clone();
            let ad_dig = ad.diagonal();

            for (k, _) in ad_dig.iter().enumerate().filter(|(_, e)| **e > 0) {
                let mut x = Vec::new();
                x.resize(i + 1, HashSet::new());

                let mut y = Vec::new();
                y.resize(i + 1, HashSet::new());

                let mut z = Vec::new();
                z.resize(i + 1, HashSet::new());

                x[0] = vec![b[k].clone()].into_iter().collect();
                y[i] = vec![b[k].clone()].into_iter().collect();

                let mut j = 1;
                let mut h = i - 1;

                while j <= i {
                    let mut tmp = HashSet::new();
                    for el in x[j - 1].iter() {
                        if let Some(v) = post.get(el) {
                            tmp.extend(v.iter().cloned());
                        }
                    }

                    x[j] = tmp;

                    j += 1;


                    let mut tmp = HashSet::new();

                    for el in y[h + 1].iter() {
                        if let Some(v) = pre.get(el) {
                            tmp.extend(v.iter().cloned());
                        }
                    }

                    y[h] = tmp;

                    if h != 0 { h -= 1 };
                }

                if !x[i].contains(&b[k]) || !y[0].contains(&b[k]) {
                    continue;
                }

                for m in (0..=i).into_iter() {
                    z[m] = x[m].intersection(&y[m]).cloned().collect();
                }

                let d = &z[0];
                let mut v = vec![d.iter().next().unwrap().clone()];
                let mut d = v[0].clone();

                for m in (1..i).into_iter() {
                    let intersection = post.get(&d).unwrap().intersection(&z[m]).cloned().collect::<Vec<_>>();
                    v.push(intersection[0].clone());
                    d = v.last().unwrap().clone();
                }


                //v.unique();
                if v[0..(i / 2)].to_vec().eq(&v[(i / 2)..i].to_vec()) {
                    println!("EQ {:?} == {:?}", v[0..(i / 2)].to_vec(), v[(i / 2)..i].to_vec());
                    loops.push(v[0..(i / 2)].to_vec());
                }
                else {
                    loops.push(v);
                }

            }
        }

        loops.sort_by(|a, b| b.len().cmp(&a.len()));
        loops.first().cloned()
    }

    pub fn get_part(&self) -> Option<Vec<Vertex>> {
        let mut result = Vec::new();

        for vertex in self.elements.iter().filter(|v| matches!(v, Vertex::Position(_))) {

            if self.connections.iter().find(|conn| conn.second().eq(vertex)).is_some() {
                continue;
            }

            result.push(vertex.clone());

            while let Some(conn) = self.connections.iter().find(|conn| conn.first() == result.last().unwrap()) {
                result.push(conn.second().clone());
            }

            break
        }

        if result.is_empty() { None } else { Some(result) }
    }

    pub fn remove_part(&mut self, part: &Vec<Vertex>) {
        for loop_element in part.iter() {
            if !self.elements.contains(loop_element) {
                continue
            }

            let no_in_loop = self
                .connections
                .iter()
                .filter(|conn| {
                    (loop_element.is_position() && conn.first().eq(loop_element) && !part.contains(&conn.second()))
                        ||  (loop_element.is_position() &&  conn.second().eq(loop_element) && !part.contains(&conn.first()))
                }).cloned().collect::<Vec<_>>();

            println!("{:?} -> {:?}", loop_element, no_in_loop);

            self
                .connections
                .drain_filter(|conn| {
                    !no_in_loop.contains(conn)
                        &&
                        (conn.first().eq(loop_element) || conn.second().eq(loop_element))
                }).count();

            if self.connections.iter().find(|conn| conn.first().eq(loop_element) || conn.second().eq(loop_element)).is_none() {
                self.elements.remove(self.elements.iter().position(|el| el.eq(loop_element)).unwrap());
            }
        }

        println!("RES: {:?}", self);
    }
}