use std::collections::HashMap;
use std::iter::FromIterator;

use net::PetriNet;
use net::vertex::VertexIndex;

#[derive(Debug, PartialEq)]
pub enum NodeMark {
    None,
    Temporary,
    Permanent
}

#[derive(Debug, Default)]
pub struct NetPaths {
    paths: Vec<Vec<VertexIndex>>
}

impl NetPaths {

    /// Find all paths from input positions to output positions if exists
    pub fn find(net: &PetriNet) -> NetPaths {
        // get input positions
        let input_positions = net.positions
            .iter()
            .filter(|(index, _)| {
                net.connections.iter().find(|c| c.second() == **index).is_none()
            })
            .map(|(index, _)| *index)
            .collect::<Vec<_>>();

        // get output positions
        let output_positions = net.positions
            .iter()
            .filter(|(index, _)| {
                net.connections.iter().find(|c| c.first() == **index).is_none()
            })
            .map(|(index, _)| *index)
            .collect::<Vec<_>>();

        if input_positions.is_empty() || output_positions.is_empty() {
            return NetPaths::default()
        }

        // DFS Top Sort
        let mut visited = HashMap::from_iter(
            net.vertices().iter().map(|v| (*v, NodeMark::None))
        );
        let mut paths = vec![];
        let mut stack = vec![];
        for vert in input_positions.iter() {
            visited.values_mut().for_each(|m| *m = NodeMark::None);
            if let Err(_) = NetPaths::process_dfs(net, *vert, &mut visited, &mut paths, &mut stack) {
                log::error!("Cycle detected");
                return NetPaths::default()
            }
        }

        NetPaths { paths }
    }

    pub fn process_dfs(net: &PetriNet,
                       vertex: VertexIndex,
                       visited: &mut HashMap<VertexIndex, NodeMark>,
                       paths: &mut Vec<Vec<VertexIndex>>,
                       stack: &mut Vec<VertexIndex>) -> Result<(), ()>
    {
        if visited[&vertex] == NodeMark::Permanent {
            return Ok(())
        } else if visited[&vertex] == NodeMark::Temporary {
            return Err(()) // cycle detected
        }

        stack.push(vertex);

        let adjacent = net.adjacent(vertex);
        if adjacent.is_empty() {
            paths.push(vec![]);
            for &v in stack.iter() {
                paths.last_mut().unwrap().push(v);
            }
        }

        *visited.get_mut(&vertex).unwrap() = NodeMark::Temporary;
        for vert in adjacent {
            NetPaths::process_dfs(net, vert, visited, paths, stack)?;
        }

        *visited.get_mut(&vertex).unwrap() = NodeMark::Permanent;
        stack.pop();
        Ok(())
    }

    pub fn get_longest(&self) -> Option<&[VertexIndex]> {
        self.paths.iter()
            .max_by(|a, b| a.len().cmp(&b.len()))
            .map(|a| a.as_slice())
    }
}

#[cfg(test)]
mod tests {
    use core::graph_utils::path::NetPaths;
    use net::PetriNet;

    #[test]
    fn test_find_path() {
        let mut net = PetriNet::new();
        let p1 = net.add_position(1).index();
        let p2 = net.add_position(2).index();
        let p3 = net.add_position(3).index();
        let p4 = net.add_position(4).index();
        let p5 = net.add_position(5).index();

        let t1 = net.add_transition(1).index();
        let t2 = net.add_transition(2).index();

        net.connect(p1, t1);
        net.connect(t1, p2);
        net.connect(p2, t2);
        net.connect(t2, p3);
        net.connect(p4, t2);
        net.connect(t2, p5);

        NetPaths::find(&net);
    }
}