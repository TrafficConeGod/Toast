#include "Stack.h"
using namespace toast;

Stack::~Stack() {
    for (State* state : states) {
        delete state;
    }
}

State* Stack::get_state() {
    return states.back();
}

void Stack::push_state(State* state) {
    states.push_back(state);
    // std::cout << "PUSH: " << states.size() << std::endl;
}

State* Stack::pop_state() {
    // std::cout << "POP: " << states.size() << std::endl;
    State* state = states.back();
    states.pop_back();
    return state;
}

bool Stack::is_empty() {
    return states.size() == 0;
}