#include "Instruction.h"
using namespace toast;

Instruction::Instruction(InstructionType type, std::vector<int> args) {
    this->type = type;
    this->args = args;
}

InstructionType Instruction::get_type() {
    return type;
}

std::vector<int> Instruction::get_args() {
    return args;
}

std::string Instruction::make_human_readable() {
    std::stringstream stream;
    switch (get_type()) {
        case InstructionType::PUSH:
            stream << "PUSH";
            break;
        case InstructionType::POP:
            stream << "POP";
            break;
        case InstructionType::SET:
            stream << "SET";
            break;
        case InstructionType::MOVE:
            stream << "MOVE";
            break;
        case InstructionType::CALL:
            stream << "CALL";
            break;
        case InstructionType::EXIT:
            stream << "EXIT";
            break;
        case InstructionType::FRAME:
            stream << "FRAME";
            break;
        case InstructionType::BACK:
            stream << "BACK";
            break;
        case InstructionType::SKIP:
            stream << "SKIP";
            break;
        case InstructionType::ARG:
            stream << "ARG";
            break;
        case InstructionType::RETURN:
            stream << "RETURN";
            break;
        case InstructionType::MOVE_RETURN:
            stream << "MOVE_RETURN";
            break;
        case InstructionType::ADD:
            stream << "ADD";
            break;
        case InstructionType::SUBTRACT:
            stream << "SUBTRACT";
            break;
        case InstructionType::MULTIPLY:
            stream << "MULTIPLY";
            break;
        case InstructionType::DIVIDE:
            stream << "DIVIDE";
            break;
        case InstructionType::EQUALS:
            stream << "EQUALS";
            break;
        case InstructionType::IF:
            stream << "IF";
            break;
        case InstructionType::LENGTH:
            stream << "LENGTH";
            break;
        case InstructionType::STREAM_IN:
            stream << "STREAM_IN";
            break;
        case InstructionType::STREAM_OUT:
            stream << "STREAM_OUT";
            break;
        case InstructionType::MOVE_IN:
            stream << "MOVE_IN";
            break;
        case InstructionType::MOVE_OUT:
            stream << "MOVE_OUT";
            break;
        case InstructionType::DELETE:
            stream << "DELETE";
            break;
        default:
            throw Exception("No name for instruction type");
    }
    for (int j = 0; j < get_args().size(); j++) {
        int arg = get_args()[j];
        stream << " " << arg;
    }
    stream << std::endl;
    return stream.str();
}

Instruction::Instruction(InstructionType type, std::vector<int> args, std::string str) {
    this->type = type;
    this->args = args;
    this->str = str;
}

std::string Instruction::get_string() {
    return str;
}
