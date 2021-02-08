#pragma once
#include "State.h"
#include <iostream>
#include <vector>

namespace toast {
    class Stack {
        private:
            std::vector<State*> states;
        public:
            ~Stack();
            void push_state(State* state);
            State* pop_state();
            State* get_state(int offset);
            bool is_empty();
    };
}