#include "Frame.h"
using namespace toast;

Frame::Frame() {}

Frame::Frame(std::map<uint, State*> states) {
    this->states = states;
}

Frame::~Frame() {
    
}

void Frame::delete_states() {
    std::map<uint, State*>::iterator it;
    for (it = states.begin(); it != states.end(); it++) {
        delete it->second;
    }
}

void Frame::push_state(uint state_key) {
    states[state_key] = new State();
}

State* Frame::pop_state(uint state_key) {
    State* state = states[state_key];
    states.erase(state_key);
    return state;
}

State* Frame::get_state(uint state_key) {
    return states[state_key];
}

Frame* Frame::clone() {
    return new Frame(states);
}