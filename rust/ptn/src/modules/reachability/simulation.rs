use indexmap::IndexMap;
use net::vertex::VertexIndex;
use crate::net::PetriNet;

#[repr(C)]
pub struct UpdateMarking {
    vertex: VertexIndex,

    // How much take or put
    marking: usize,
}

pub struct Simulation {
    net: *const PetriNet,
    marking: IndexMap<VertexIndex, usize>,
    fired: Vec<VertexIndex>,
    // Cycles
    cycles: usize,
}

impl Simulation {
    pub fn new(net: *const PetriNet) -> Simulation {
        Simulation {
            net,
            marking: {
                let net = unsafe { &*net };
                let mut marking = IndexMap::new();
                for (index, vertex) in net.positions().iter() {
                    marking.insert(*index, vertex.markers());
                }
                marking
            },
            fired: vec![],
            cycles: 0,
        }
    }

    pub fn net(&self) -> &PetriNet {
        unsafe { &*self.net }
    }

    /// Возвращает количество сработавших переходов
    pub fn simulate(&mut self) -> i32 {
        // Для каждого перехода
        //   Получить входящие соединения
        //   Получить исходящие соединения
        //   Проверить, что все входящие соединения активны по весу
        //   Если да, то активировать переход

        let mut fired = 0;
        let mut fired_transitions = Vec::new();
        let mut take_marking = self.marking.clone();
        take_marking.iter_mut().for_each(|(_, v)| *v = 0);

        let mut put_marking = self.marking.clone();
        put_marking.iter_mut().for_each(|(_, v)| *v = 0);

        for (index, _) in self.net().transitions().iter() {
            let input = self
                .net()
                .connections()
                .iter()
                .filter(|c| c.second() == *index)
                .collect::<Vec<_>>();

            let output = self
                .net()
                .connections()
                .iter()
                .filter(|c| c.first() == *index)
                .collect::<Vec<_>>();

            let mut can_fire = true;
            for connection in input.iter() {
                let marking_at = *self.marking.get(&connection.first()).unwrap();
                let take_marking_at = *take_marking.get(&connection.first()).unwrap();
                if (marking_at - take_marking_at) < connection.weight() {
                    can_fire = false;
                    break;
                }
            }

            if can_fire {
                let max_take = input.iter().map(|c| c.weight()).max().unwrap_or(0);

                for connection in input.iter() {
                    *take_marking.get_mut(&connection.first()).unwrap() += max_take;
                }

                for connection in output.iter() {
                    *put_marking.get_mut(&connection.second()).unwrap() += connection.weight();
                }

                fired_transitions.push(*index);
                fired += 1;
            }
        }

        // take and put in self.marking
        for (index, take) in take_marking.iter() {
            let put = *put_marking.get(index).unwrap();
            let mark = *self.marking.get(index).unwrap();
            *self.marking.get_mut(index).unwrap() = mark - take + put;
        }
        self.fired = fired_transitions;

        if fired > 0 {
            self.cycles += 1;
        }

        fired
    }
    
    pub fn markers_at(&self, idx: VertexIndex) -> usize {
        // todo unwrap
        self.marking.get(&idx).copied().unwrap()
    }

    pub fn cycles(&self) -> usize {
        self.cycles
    }

    pub fn fired(&self) -> &[VertexIndex] {
        &self.fired
    }
}
