use net::vertex::VertexIndex;
use net::PetriNet;
use std::collections::HashMap;

#[derive(Debug, PartialEq)]
enum DFSVertexState {
    NotVisited,
    InStack,
    Done,
}

#[derive(Debug)]
pub struct NetCycles {
    cycles: Vec<Vec<VertexIndex>>,
}

impl NetCycles {
    pub fn find(net: &PetriNet) -> NetCycles {
        let vertices = net.vertices();
        let mut cycles = vec![];

        let mut visited = HashMap::new();
        for &index in vertices.iter() {
            visited.insert(index, DFSVertexState::NotVisited);
        }

        for &vert in vertices.iter() {
            if visited[&vert] == DFSVertexState::NotVisited {
                let mut stack = vec![vert];
                *visited.get_mut(&vert).unwrap() = DFSVertexState::InStack;
                NetCycles::process_dfs_tree(net, &mut stack, &mut visited, &mut cycles);
            }
        }

        NetCycles { cycles }
    }

    fn process_dfs_tree(
        net: &PetriNet,
        stack: &mut Vec<VertexIndex>,
        visited: &mut HashMap<VertexIndex, DFSVertexState>,
        cycles: &mut Vec<Vec<VertexIndex>>,
    ) {
        for vert in net.adjacent(stack[stack.len() - 1]) {
            match visited[&vert] {
                DFSVertexState::InStack => {
                    // print cycle
                    let mut stack2 = vec![stack.pop().unwrap()];
                    while stack2[stack2.len() - 1] != vert {
                        stack2.push(stack.pop().unwrap());
                    }
                    cycles.push(vec![]);
                    while !stack2.is_empty() {
                        let vertex = stack2.pop().unwrap();
                        cycles.last_mut().unwrap().push(vertex);
                        stack.push(vertex);
                    }
                }
                DFSVertexState::NotVisited => {
                    stack.push(vert);
                    *visited.get_mut(&vert).unwrap() = DFSVertexState::InStack;
                    NetCycles::process_dfs_tree(net, stack, visited, cycles);
                }
                DFSVertexState::Done => {}
            }
        }
        *visited.get_mut(stack.last().unwrap()).unwrap() = DFSVertexState::Done;
        stack.pop();
    }

    pub fn get_longest(&self) -> Option<&[VertexIndex]> {
        self.cycles
            .iter()
            .max_by(|a, b| a.len().cmp(&b.len()))
            .map(|v| v.as_slice())
    }
}

#[cfg(test)]
mod tests {
    use super::NetCycles;
    use net::PetriNet;
    use net::vertex::VertexIndex;

    #[test]
    fn test_dfs() {
        let mut net = PetriNet::new();
        let p1 = net.add_position(1).index();
        let p2 = net.add_position(2).index();
        let p3 = net.add_position(3).index();

        let t1 = net.add_transition(1).index();
        let t2 = net.add_transition(2).index();
        let t3 = net.add_transition(3).index();

        net.connect(p1, t1, 1);
        net.connect(t1, p2, 1);
        net.connect(p2, t2, 1);
        net.connect(t2, p3, 1);
        net.connect(p3, t3, 1);
        net.connect(t3, p1, 1);

        let cycles = NetCycles::find(&net);
        assert_eq!(cycles.cycles.len(), 1);

        let cycle = cycles.cycles.get(0).unwrap();
        assert_eq!(cycle.len(), 6);
        assert_eq!(cycle[0], VertexIndex::position(1));
        assert_eq!(cycle[1], VertexIndex::transition(1));
        assert_eq!(cycle[2], VertexIndex::position(2));
        assert_eq!(cycle[3], VertexIndex::transition(2));
        assert_eq!(cycle[4], VertexIndex::position(3));
        assert_eq!(cycle[5], VertexIndex::transition(3));
    }
}
