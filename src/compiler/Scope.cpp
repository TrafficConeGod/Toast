#include "Scope.h"
#include <algorithm>
using namespace toast;

Scope::Scope(ScopeType type, int stack_frame) {
    this->type = type;
    this->stack_frame = stack_frame;
}

Scope::~Scope() {
    for (Var* var : var_stack) {
        delete var;
    }
}

void Scope::push(Var* var) {
    return var_stack.push_back(var);
}

std::vector<Var*> Scope::get_var_stack() {
    return var_stack;
}

void Scope::push_var(std::string ident, Var* var) {
    var_map[ident] = var;
    var_stack.push_back(var);
}

bool Scope::has_var(std::string ident) {
    return var_map.count(ident) != 0;
}

bool Scope::has_var(Var* var) {
    // google was real helpful here
    bool val = false;
    std::for_each(var_map.begin(), var_map.end(), [&](const std::pair<std::string, Var*> entry) {
        if (entry.second == var) {
            val = true;
        }
    });
    return val;
}

Var* Scope::get_var(std::string ident) {
    return var_map[ident];
}

ScopeType Scope::get_type() {
    return type;
}

int Scope::get_frame() {
    return stack_frame;
}

std::vector<Instruction> Scope::get_instructions() {
    std::vector<Instruction> instructions;
    for (Var* var : var_stack) {
        instructions.push_back(Instruction(InstructionType::POP, { var->get_key() }));
    }
    return instructions;
}