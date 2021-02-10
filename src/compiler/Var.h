#pragma once
#include "../shared/StateTypeHolder.h"
#include <iostream>
#include <vector>

namespace toast {
    class Var {
        private:
            std::vector<StateTypeHolder> type;
            int stack_frame;
            int offset;
        public:
            Var(StateTypeHolder type, int stack_frame);
            void set_stack_frame();
            StateTypeHolder get_type();
            int get_frame();
            void set_offset(int offset);
            int get_offset();
            std::vector<int> get_args();
    };
}