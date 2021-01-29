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

void toast::StateTypeHolder::func_init(StateTypeHolder return_type, std::vector<StateTypeHolder> func_args) {
    this->return_type.push_back(return_type);
    this->func_args = func_args;
}

toast::StateType toast::StateTypeHolder::get_main_type() {
    return main_type;
}

toast::StateTypeHolder toast::StateTypeHolder::get_return_type() {
    return return_type.back();
}

std::vector<toast::StateTypeHolder> toast::StateTypeHolder::get_func_args() {
    return func_args;
}

bool toast::StateTypeHolder::equals(StateTypeHolder type) {
    return main_type == type.get_main_type();
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

std::string toast::make_human_readable(Instruction instruction) {
    std::stringstream stream;
    switch (instruction.get_type()) {
        case PUSH:
            stream << "PUSH";
            break;
        case POP:
            stream << "POP";
            break;
        case SET:
            stream << "SET";
            break;
        case MOVE:
            stream << "MOVE";
            break;
        case CALL:
            stream << "CALL";
            break;
        case EXIT:
            stream << "EXIT";
            break;
        case FRAME:
            stream << "FRAME";
            break;
        case BACK:
            stream << "BACK";
            break;
        case SKIP:
            stream << "SKIP";
            break;
        default:
            throw Exception("No name for instruction type");
    }
    for (int j = 0; j < instruction.get_args().size(); j++) {
        int arg = instruction.get_args()[j];
        stream << " " << arg;
    }
    stream << std::endl;
    return stream.str();
}