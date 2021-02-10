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
            int stack_frame = 0;
            int temp_offset = 0;
        public:
            Builder(Script* script);
            ~Builder();
            std::vector<Instruction> get_instructions();
            bool has_var(std::string ident);
            int get_var_offset(Var* var);
            Var* get_var(std::string ident);
            int get_frame();
            void push_var(std::string ident, Var* var);
            void update_var(Var* var);
            void add_temp_offset();
            void sub_temp_offset();
            int get_temp_offset();
            void push_scope(Scope* scope);
            Scope* pop_scope();
    };
}