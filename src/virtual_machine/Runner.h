#pragma once
#include "Frame.h"
#include "../shared/State.h"
#include "StateArray.h"
#include "../shared/Instruction.h"
#include "../shared/StateTypeHolder.h"
#include <iostream>
#include <vector>
#include <map>
#include <deque>

namespace toast {
    class Runner {
        private:
            uint position = 0;
            std::vector<Instruction> instructions;
            std::vector<Frame*> frames;
            std::vector<uint> return_stack;
            std::deque<State*> call_args;
            State* return_state;
        public:
            Runner(std::vector<Instruction>);
            ~Runner();
            void handle_instruction();
            // void set_frame(int frame_key, bool push_stack);
            // State* get_state(int frame_key, int offset);
            void push_state(uint state_key);
            State* pop_state(uint state_key);
            State* get_state(uint state_key);
            std::vector<State*> get_states(Instruction instruction);
    };
}