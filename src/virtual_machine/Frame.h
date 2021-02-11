#pragma once
#include "../shared/State.h"
#include "Stack.h"
#include <iostream>
#include <vector>
#include <map>

namespace toast {
    class Frame {
        private:
            // std::map<uint, Stack*> stacks;
            std::map<uint, State*> states;
        public:
            Frame();
            Frame(std::map<uint, State*> states);
            ~Frame();
            void delete_states();
            void push_state(uint state_key);
            State* pop_state(uint state_key);
            State* get_state(uint state_key);
            Frame clone();
            Frame* clone_ptr();
    };
}