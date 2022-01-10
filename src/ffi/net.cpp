//
// Created by Николай Муравьев on 10.01.2022.
//

#include "../../include/ffi/net.h"
#include "../../include/elements/position.h"
#include "../../include/elements/transition.h"

void PetriNet::remove_object(PetriObject *object) {
    if (auto position = dynamic_cast<Position*>(object); position) {
        this->remove_position(position->position());
    }
    else if (auto transition = dynamic_cast<Transition*>(object); transition) {
        this->remove_transition(transition->transition());
    }
}
