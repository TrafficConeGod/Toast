#pragma once
#include "Frame.h"
#include "State.h"
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
            int position = 0;
            int frame_key = -1;
            std::vector<Instruction> instructions;
            std::map<int, Frame*> frames;
            std::vector<int> return_stack;
            std::deque<State*> call_args;
            State* return_state;
        public:
            Runner(std::vector<Instruction>);
            ~Runner();
            void handle_instruction();
            void set_frame(int frame_key, bool push_stack);
            State* get_state(int frame_key, int offset);
            State* push_state(StateTypeHolder type);
            State* pop_state();
    };
}