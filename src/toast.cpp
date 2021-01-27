#include "toast.h"

toast::Exception::Exception(const char* msg) {
    this->msg = msg;
}

const char* toast::Exception::what() const throw() {
    return msg;
}


toast::StateTypeHolder::StateTypeHolder(StateType main_type) {
    this->main_type = main_type;
}

void toast::StateTypeHolder::func_init(StateTypeHolder* return_type, std::vector<StateTypeHolder*> func_args) {
    this->return_type = return_type;
    this->func_args = func_args;
}

toast::StateType toast::StateTypeHolder::get_main_type() {
    return main_type;
}

toast::StateTypeHolder* toast::StateTypeHolder::get_return_type() {
    return return_type;
}

std::vector<toast::StateTypeHolder*> toast::StateTypeHolder::get_func_args() {
    return func_args;
}

bool toast::StateTypeHolder::equals(StateTypeHolder* type) {
    return main_type == type->get_main_type();
}

bool toast::StateTypeHolder::equals(StateType type) {
    return main_type == type;
}

toast::Instruction::Instruction(InstructionType type, std::vector<int> args) {
    this->type = type;
    this->args = args;
}

toast::InstructionType toast::Instruction::get_type() {
    return type;
}

std::vector<int> toast::Instruction::get_args() {
    return args;
}