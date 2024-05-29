mod cycle;
mod path;

use std::collections::HashSet;
use net::PetriNet;
use net::vertex::{VertexIndex, VertexType};
pub use self::cycle::*;
pub use self::path::*;

/// Get adjacent vertices to the given vertex
fn adjacent(net: &PetriNet, idx: VertexIndex) -> Vec<VertexIndex> {
    let adjacent = net.edges()
        .directed()
        .iter()
        .filter(|edge| edge.begin() == idx)
        .map(|edge| edge.end())
        .fold(HashSet::new(), |mut set, idx| {
            set.insert(idx);
            set
        });
    
    let adjacent = net.edges()
        .inhibitor()
        .iter()
        .filter(|edge| {
            match idx.type_ { 
                VertexType::Position => edge.place() == idx,
                VertexType::Transition => edge.transition() == idx
            }
        })
        .map(|edge| {
            match idx.type_ {
                VertexType::Position => edge.transition(),
                VertexType::Transition => edge.place()
            }
        })
        .fold(adjacent, |mut set, idx| {
            set.insert(idx);
            set
        });
    
    adjacent.into_iter().collect()
}

#[cfg(test)]
mod tests {

    use net::{DirectedEdge, PetriNet};
    
    #[test]
    fn test_adjacent() {
        let mut net = PetriNet::new();
        let p1 = net.add_position(1).index();
        let p2 = net.add_position(2).index();
        let t1 = net.add_transition(1).index();
        let t2 = net.add_transition(2).index();
        
        net.add_directed(DirectedEdge::new(p1, t1));
        net.add_directed(DirectedEdge::new(t1, p2));
        net.add_directed(DirectedEdge::new(p2, t2));
        net.add_directed(DirectedEdge::new(t2, p1));
        
        let adjacent = super::adjacent(&net, p1);
        assert_eq!(adjacent.len(), 2);
        assert!(adjacent.contains(&t1));
        assert!(adjacent.contains(&t2));
        
        let adjacent = super::adjacent(&net, t1);
        assert_eq!(adjacent.len(), 2);
        assert!(adjacent.contains(&p2));
        assert!(adjacent.contains(&p1));
    }
}