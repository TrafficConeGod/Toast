#pragma once
#include "State.h"
#include "Stack.h"
#include <iostream>
#include <vector>

namespace toast {
    class Frame {
        private:
            int last;
            std::vector<int> keys;
            std::vector<Stack*> stacks;
        public:
            ~Frame();
            void push_stack(int key);
            void pop_stack();
            void push_state(State* state);
            State* pop_state();
            State* get_state(int offset);
            int get_return();
    };
}