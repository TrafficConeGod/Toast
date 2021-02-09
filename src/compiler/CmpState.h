#pragma once
#include "../shared/StateTypeHolder.h"
#include <iostream>
#include <vector>

namespace toast {
    class CmpState {
        private:
            std::vector<StateTypeHolder> type;
            int stack_frame;
            int offset;
        public:
            CmpState(StateTypeHolder type, int stack_frame);
            void set_stack_frame();
            StateTypeHolder get_type();
            int get_frame();
            void set_offset(int offset);
            int get_offset();
            std::vector<int> get_args();
    };
}