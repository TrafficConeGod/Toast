#include "Scope.h"
#include <algorithm>
using namespace toast;

Scope::Scope(ScopeType type) {
    this->type = type;
}

Scope::Scope(ScopeType type, StateTypeHolder function_type) {
    this->type = type;
    this->function_type.push_back(function_type);
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

std::vector<Instruction> Scope::get_instructions() {
    std::vector<Instruction> instructions;
    for (Var* var : var_stack) {
        instructions.push_back(Instruction(InstructionType::POP, { var->get_key() }));
    }
    return instructions;
}

StateTypeHolder Scope::get_function_type() {
    return function_type.back();
}