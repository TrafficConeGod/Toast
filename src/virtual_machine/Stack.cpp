#include "Stack.h"
using namespace toast;
using namespace std;

Stack::~Stack() {
    for (State* state : states) {
        delete state;
    }
}

State* Stack::get_state(int offset) {
    State* state = states[(states.size() - 1) - offset];
    return state;
}

void Stack::push_state(State* state) {
    states.push_back(state);
    // cout << "PUSH: " << states.size() << endl;
}

State* Stack::pop_state() {
    // cout << "POP: " << states.size() << endl;
    State* state = states.back();
    states.pop_back();
    return state;
}

bool Stack::is_empty() {
    return states.size() == 0;
}