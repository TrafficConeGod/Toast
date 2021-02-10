#pragma once
#include "../shared/State.h"
#include <iostream>
#include <vector>

namespace toast {
    class StateArray {
        private:
            std::vector<State*> states_holding;
            std::vector<State*> states;
        public:
            ~StateArray();
            StateArray();
            int get_length();
            void push_state(State* state);
            State* get_back();
            State* pop_state();
            void set_state(int index, State* state);
            State* get_state(int index);
    };
}