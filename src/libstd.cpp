#include <iostream>
#include "toast.h"
using namespace toast;
using namespace std;

State* print(State* msg_state) {
    std::string msg = msg_state->get_cpp_string();
    std::cout << msg;
    return new State(toast::StateTypeHolder(VOID));
}