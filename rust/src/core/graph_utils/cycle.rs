use std::collections::HashMap;
use net::PetriNet;
use net::vertex::VertexIndex;

#[derive(Debug, PartialEq)]
enum DFSVertexState {
    NotVisited,
    InStack,
    Done
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

    fn process_dfs_tree(net: &PetriNet,
                        stack: &mut Vec<VertexIndex>,
                        visited: &mut HashMap<VertexIndex, DFSVertexState>,
                        cycles: &mut Vec<Vec<VertexIndex>>,)
    {
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
                },
                DFSVertexState::NotVisited => {
                    stack.push(vert);
                    *visited.get_mut(&vert).unwrap() = DFSVertexState::InStack;
                    NetCycles::process_dfs_tree(net, stack, visited, cycles);
                },
                DFSVertexState::Done => {}
            }
        }
        *visited.get_mut(stack.last().unwrap()).unwrap() = DFSVertexState::Done;
        stack.pop();
    }

    pub fn get_longest(&self) -> Option<&[VertexIndex]> {
        self.cycles.iter()
            .max_by(|a, b| a.len().cmp(&b.len()))
            .map(|v| v.as_slice())
    }
}

#[cfg(test)]
mod tests {
    use super::NetCycles;
    use net::PetriNet;

    #[test]
    fn test_dfs() {
        let mut net = PetriNet::new();
        let p1 = net.add_position(1).index();
        let p2 = net.add_position(2).index();
        let p3 = net.add_position(3).index();

        let t1 = net.add_transition(1).index();
        let t2 = net.add_transition(2).index();
        let t3 = net.add_transition(3).index();

        net.connect(p1, t1);
        net.connect(t1, p2);
        net.connect(p2, t2);
        net.connect(t2, p3);
        net.connect(p3, t3);
        net.connect(t3, p1);

        let cycles = NetCycles::find(&net);
        println!("{cycles:?}");
    }
}