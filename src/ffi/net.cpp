//
// Created by Николай Муравьев on 10.01.2022.
//

#include "net.h"
#include "../elements/position.h"
#include "../elements/transition.h"

void PetriNet::remove_object(PetriObject *object) {
    if (auto position = dynamic_cast<Position*>(object); position) {
        this->remove_position(position->position());
    }
    else if (auto transition = dynamic_cast<Transition*>(object); transition) {
        this->remove_transition(transition->transition());
    }
}
