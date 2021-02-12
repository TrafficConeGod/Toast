#pragma once
#include "../shared/StateTypeHolder.h"
#include <iostream>
#include <vector>

namespace toast {
    class Var {
        private:
            std::vector<StateTypeHolder> type;
            uint key;
        public:
            Var(StateTypeHolder type, uint key);
            void set_stack_frame();
            StateTypeHolder get_type();
            uint get_offset();
            std::vector<uint> get_args();
            uint get_key();
    };
}