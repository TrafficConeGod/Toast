#include "Builder.h"
#include "../shared/Instruction.h"
#include "Statement.h"
#include "Script.h"
#include "CompilerException.h"
#include "Var.h"
#include "ScopeType.h"
#include "Scope.h"
#include "t_cmp.h"
using namespace toast;

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

bool Builder::has_var(std::string ident) {
    for (int i = scope_stack.size() - 1; i >= 0; i--) {
        Scope* scope = scope_stack[i];
        if (scope->has_var(ident)) {
            return true;
        }
    }
    return false;
};

int Builder::get_var_offset(Var* var) {
    int offset = temp_offset;
    for (int i = scope_stack.size() - 1; i >= 0; i--) {
        Scope* scope = scope_stack[i];
        if (scope->has_var(var)) {
            return scope->get_var_offset(var) + offset;
        }
        if (scope->get_frame() == var->get_frame()) {
            offset += scope->get_var_stack().size();
        }
    }
    throw CompilerException();
}

Var* Builder::get_var(std::string ident) {
    for (int i = scope_stack.size() - 1; i >= 0; i--) {
        Scope* scope = scope_stack[i];
        if (scope->has_var(ident)) {
            Var* var = scope->get_var(ident);
            update_var(var);
            return var;
        }
    }
    throw CompilerException();
}

int Builder::get_frame() {
    return scope_stack.back()->get_frame();
}

void Builder::push_var(std::string ident, Var* var) {
    scope_stack.back()->push_var(ident, var);
}

void Builder::update_var(Var* var) {
    var->set_offset(get_var_offset(var));
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