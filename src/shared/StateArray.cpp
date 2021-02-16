#include "StateArray.h"
using namespace toast;


StateArray::StateArray() {
    
}

StateArray::~StateArray() {
    
}

int StateArray::get_length() {
    return states.size();
}

void StateArray::push_state(State* state) {
    states.push_back(state);
}

State* StateArray::get_back() {
    return states.back();
}

State* StateArray::pop_state() {
    State* state = states.back();
    states.pop_back();
    return state;
}
