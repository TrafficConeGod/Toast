#pragma once
#include "../shared/InstructionType.h"
#include "../shared/Instruction.h"
#include "ScopeType.h"
#include "CmpState.h"
#include <iostream>
#include <vector>
#include <map>

namespace toast {
    class Scope {
        private:
            ScopeType type;
            std::vector<CmpState*> state_stack;
            std::map<std::string, CmpState*> state_map;
            int stack_frame;
        public:
            Scope(ScopeType type, int stack_frame);
            ~Scope();
            void push(CmpState* state);
            std::vector<CmpState*> get_state_stack();
            void push_state(std::string ident, CmpState* state);
            bool has_state(std::string ident);
            bool has_state(CmpState* state);
            int get_state_offset(std::string ident);
            int get_state_offset(CmpState* state);
            CmpState* get_state(std::string ident);
            ScopeType get_type();
            int get_frame();
            std::vector<Instruction> get_instructions();
    };

}