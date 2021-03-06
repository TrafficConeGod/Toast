#pragma once
#include "Scope.h"
#include <iostream>
#include <vector>

namespace toast {
    class Script;
    class Builder {
        private:
            Script* script;
            std::vector<Instruction> instructions;
            std::vector<Scope*> scope_stack;
            uint var_key = 1;
        public:
            Builder(Script* script);
            ~Builder();
            std::vector<Instruction> get_instructions();
            bool has_var(std::string ident);
            Var* get_var(std::string ident);
            void push_var(std::string ident, Var* var);
            void push_var();
            void push_scope(Scope* scope);
            Scope* pop_scope();
            uint get_var_key();
            std::vector<Scope*> get_scopes();
    };
}