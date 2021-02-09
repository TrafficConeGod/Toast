#include "Builder.h"
#include "../shared/Instruction.h"
#include "Statement.h"
#include "Script.h"
#include "CompilerException.h"
#include "CmpState.h"
#include "ScopeType.h"
#include "Scope.h"
#include "t_cmp.h"
using namespace toast;
using State = CmpState;

Builder::Builder(Script* script) {
    Scope* global_scope = new Scope(ScopeType::GLOBAL, stack_frame);
    push_scope(global_scope);
    for (Statement statement : script->get_statements()) {
        merge(&instructions, statement.generate_instructions(this));
    }
    merge(&instructions, global_scope->get_instructions());
}

Builder::~Builder() {
    // for (Token token : tokens) {
    //     delete token;
    // }
    for (Scope* scope : scope_stack) {
        delete scope;
    }

}

std::vector<Instruction> Builder::get_instructions() {
    return instructions;
}

bool Builder::has_state(std::string ident) {
    for (int i = scope_stack.size() - 1; i >= 0; i--) {
        Scope* scope = scope_stack[i];
        if (scope->has_state(ident)) {
            return true;
        }
    }
    return false;
};

int Builder::get_state_offset(State* state) {
    int offset = temp_offset;
    for (int i = scope_stack.size() - 1; i >= 0; i--) {
        Scope* scope = scope_stack[i];
        if (scope->has_state(state)) {
            return scope->get_state_offset(state) + offset;
        }
        if (scope->get_frame() == state->get_frame()) {
            offset += scope->get_state_stack().size();
        }
    }
    throw CompilerException();
}

State* Builder::get_state(std::string ident) {
    for (int i = scope_stack.size() - 1; i >= 0; i--) {
        Scope* scope = scope_stack[i];
        if (scope->has_state(ident)) {
            State* state = scope->get_state(ident);
            update_state(state);
            return state;
        }
    }
    throw CompilerException();
}

int Builder::get_frame() {
    return scope_stack.back()->get_frame();
}

void Builder::push_state(std::string ident, State* state) {
    scope_stack.back()->push_state(ident, state);
}

void Builder::update_state(State* state) {
    state->set_offset(get_state_offset(state));
}

void Builder::add_temp_offset() {
    temp_offset++;
}

void Builder::sub_temp_offset() {
    temp_offset--;
}

int Builder::get_temp_offset() {
    return temp_offset;
}

void Builder::push_scope(Scope* scope) {
    scope_stack.push_back(scope);
}

Scope* Builder::pop_scope() {
    Scope* scope = scope_stack.back();
    scope_stack.pop_back();
    return scope;
}