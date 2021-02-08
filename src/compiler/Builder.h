#pragma once
#include "Script.h"
#include "Scope.h"
#include <iostream>

namespace toast {
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
            bool has_state(std::string ident);
            int get_state_offset(State* state);
            State* get_state(std::string ident);
            int get_frame();
            void push_state(std::string ident, State* state);
            void update_state(State* state);
            void add_temp_offset();
            void sub_temp_offset();
            int get_temp_offset();
            void push_scope(Scope* scope);
            Scope* pop_scope();
    };
}