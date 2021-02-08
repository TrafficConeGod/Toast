#include "Frame.h"
using namespace toast;
using namespace std;

Frame::~Frame() {
    for (Stack* stack : stacks) {
        delete stack;
    }
}

void Frame::push_stack(int key) {
    keys.push_back(key);
    stacks.push_back( new Stack() );
}

void Frame::pop_stack() {
    last = keys.back();
    keys.pop_back();
    delete stacks.back();
    stacks.pop_back();
}

State* Frame::get_state(int offset) {
    Stack* stack = stacks.back();
    return stack->get_state(offset);
}

void Frame::push_state(State* state) {
    Stack* stack = stacks.back();
    stack->push_state(state);
}

State* Frame::pop_state() {
    Stack* stack = stacks.back();
    State* state = stack->pop_state();
    if (stack->is_empty()) {
        pop_stack();
    }
    return state;
}

int Frame::get_return() {
    return last;
}