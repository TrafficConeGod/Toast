#include <iostream>
#include "t_inp.h"

t_inp::State* print(t_inp::State* msg_state) {
    std::string msg = msg_state->get_cpp_string();
    std::cout << msg;
    return new t_inp::State();
}