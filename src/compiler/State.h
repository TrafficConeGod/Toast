#pragma once
#include "../shared/StateTypeHolder.h"
#include "t_cmp.h"
#include <iostream>
#include <vector>

namespace toast {
    class State {
        private:
            std::vector<StateTypeHolder> type;
            int stack_frame;
            int offset;
        public:
            State(StateTypeHolder type, int stack_frame);
            void set_stack_frame();
            StateTypeHolder get_type();
            int get_frame();
            void set_offset(int offset);
            int get_offset();
            std::vector<int> get_args();
    };
}