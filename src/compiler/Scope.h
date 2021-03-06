#pragma once
#include "../shared/InstructionType.h"
#include "../shared/Instruction.h"
#include "ScopeType.h"
#include "Var.h"
#include <iostream>
#include <vector>
#include <map>

namespace toast {
    class Scope {
        private:
            ScopeType type;
            std::vector<Var*> var_stack;
            std::map<std::string, Var*> var_map;
            std::vector<StateTypeHolder> function_type;
        public:
            Scope(ScopeType type);
            Scope(ScopeType type, StateTypeHolder function_type);
            ~Scope();
            void push(Var* var);
            std::vector<Var*> get_var_stack();
            void push_var(std::string ident, Var* var);
            bool has_var(std::string ident);
            bool has_var(Var* var);
            Var* get_var(std::string ident);
            ScopeType get_type();
            std::vector<Instruction> get_instructions();
            StateTypeHolder get_function_type();
    };

}