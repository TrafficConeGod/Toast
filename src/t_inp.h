#include <iostream>

namespace t_inp {
    typedef unsigned char byte;

    class State {
        private:
            byte* value;
        public:
            std::string get_cpp_string();
    };
}