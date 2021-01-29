#include <iostream>
#include "t_vm.h"

t_vm::State* print(t_vm::State* msg_state) {
    std::string msg = msg_state->get_cpp_string();
    std::cout << msg;
    return new t_vm::State(toast::StateTypeHolder(toast::VOID));
}