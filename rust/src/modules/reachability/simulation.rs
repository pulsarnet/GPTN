use indexmap::IndexMap;
use net::vertex::VertexIndex;
use ::{CVec, PetriNet};

#[repr(C)]
pub struct UpdateMarking {
    vertex: VertexIndex,

    // How much take or put
    marking: usize,
}

#[repr(C)]
pub struct FiredTransition {
    transition: VertexIndex,
    input: CVec<UpdateMarking>,
    output: CVec<UpdateMarking>,
}

pub struct Simulation {
    net: *const PetriNet,
    marking: IndexMap<VertexIndex, usize>,
}

impl Simulation {
    fn new(net: *const PetriNet) -> Simulation {
        Simulation {
            net,
            marking: {
                let net = unsafe { &*net };
                let mut marking = IndexMap::new();
                for (index, vertex) in net.positions.iter() {
                    marking.insert(*index, vertex.markers());
                }
                marking
            }
        }
    }

    fn net(&self) -> &PetriNet {
        unsafe { &*self.net }
    }

    fn simulate(&mut self) {
        // Для каждого перехода
        //   Получить входящие соединения
        //   Получить исходящие соединения
        //   Проверить, что все входящие соединения активны по весу
        //   Если да, то активировать переход
        let mut new_marking = self.marking.clone();

        for (index, transition) in self.net().transitions.iter() {
            let input = self.net().connections
                .iter()
                .filter(|c| c.second() == *index)
                .collect::<Vec<_>>();

            let output = self.net().connections
                .iter()
                .filter(|c| c.first() == *index)
                .collect::<Vec<_>>();

            let mut can_fire = true;
            for connection in input.iter() {
                if *self.marking.get(&connection.first()).unwrap() < connection.weight() {
                    can_fire = false;
                    break;
                }
            }

            if can_fire {
                // Change self.marking
                for connection in input.iter() {
                    let value = self.marking.get(&connection.first()).unwrap();
                    *new_marking.get_mut(&connection.first()).unwrap() = value - connection.weight();
                }

                for connection in output.iter() {
                    let value = self.marking.get(&connection.second()).unwrap();
                    *new_marking.get_mut(&connection.second()).unwrap() = value + connection.weight();
                }
            }
        }

        self.marking = new_marking;
    }
}

// Extern to C
// 1. Create simulation
// 2. Simulate step
// 3. Get markers
// 3. drop simulation

#[no_mangle]
pub extern "C" fn create_simulation(net: *const PetriNet) -> *mut Simulation {
    Box::into_raw(Box::new(Simulation::new(net)))
}

#[no_mangle]
pub unsafe extern "C" fn simulation_simulate(simulation: *mut Simulation) {
    let simulation = &mut *simulation;
    simulation.simulate();
}

#[no_mangle]
pub unsafe extern "C" fn simulation_markers(simulation: *const Simulation, index: VertexIndex) -> usize {
    let simulation = &*simulation;
    *simulation.marking.get(&index).unwrap()
}

#[no_mangle]
pub extern "C" fn drop_simulation(simulation: *mut Simulation) {
    let _simulation = unsafe { Box::from_raw(simulation) };
}