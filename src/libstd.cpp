#include <iostream>
#include "toast.h"
using namespace toast;

State* print(State* msg_state) {
    std::string msg = msg_state->get_cpp_string();
    std::cout << msg;
    return new State(StateTypeHolder(StateType::VOID));
}