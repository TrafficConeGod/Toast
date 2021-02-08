#pragma once
#include "State.h"
#include "ScopeType.h"
#include <iostream>
#include <vector>
#include <map>

namespace toast {
    class Scope {
        private:
            ScopeType type;
            std::vector<State*> state_stack;
            std::map<std::string, State*> state_map;
            int stack_frame;
        public:
            Scope(ScopeType type, int stack_frame);
            ~Scope();
            void push(State* state);
            std::vector<State*> get_state_stack();
            void push_state(std::string ident, State* state);
            bool has_state(std::string ident);
            bool has_state(State* state);
            int get_state_offset(std::string ident);
            int get_state_offset(State* state);
            State* get_state(std::string ident);
            ScopeType get_type();
            int get_frame();
            std::vector<Instruction> get_instructions();
    };

}