#include "Scope.h"
using namespace toast;
using namespace std;

Scope::Scope(ScopeType type, int stack_frame) {
    this->type = type;
    this->stack_frame = stack_frame;
}

Scope::~Scope() {
    for (State* state : state_stack) {
        delete state;
    }
}

void Scope::push(State* state) {
    return state_stack.push_back(state);
}

std::vector<State*> Scope::get_state_stack() {
    return state_stack;
}

void Scope::push_state(std::string ident, State* state) {
    state_map[ident] = state;
    state_stack.push_back(state);
}

bool Scope::has_state(std::string ident) {
    return state_map.count(ident) != 0;
}

bool Scope::has_state(State* state) {
    // google was real helpful here
    bool val = false;
    std::for_each(state_map.begin(), state_map.end(), [&](const std::pair<std::string, State*> entry) {
        if (entry.second == state) {
            val = true;
        }
    });
    return val;
}

int Scope::get_state_offset(std::string ident) {
    State* state = state_map[ident];
    return get_state_offset(state);
}

int Scope::get_state_offset(State* state) {
    for (int i = 0; i < state_stack.size(); i++) {
        State* check_state = state_stack[i];
        if (state == check_state) {
            return (state_stack.size() - 1) - i;
        }
    }
    return 0;
}

State* Scope::get_state(std::string ident) {
    return state_map[ident];
}

ScopeType Scope::get_type() {
    return type;
}

int Scope::get_frame() {
    return stack_frame;
}

std::vector<Instruction> Scope::get_instructions() {
    std::vector<Instruction> instructions;
    for (State* state : state_stack) {
        instructions.push_back(Instruction(POP, {}));
    }
    return instructions;
}