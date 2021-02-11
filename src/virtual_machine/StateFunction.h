#pragma once
#include "Frame.h"

namespace toast {
    class StateFunction {
        private:
            uint position;
            Frame frame;
        public:
            StateFunction(uint position, Frame frame);
            ~StateFunction();
            uint get_position();
            Frame get_frame();
    };
}