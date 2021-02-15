#include "Instruction.h"
using namespace toast;

Instruction::Instruction(InstructionType type, std::vector<uint> args) {
    this->type = type;
    this->args = args;
}

Instruction::Instruction(InstructionType type, std::vector<uint> args, std::vector<State*> states) {
    this->type = type;
    this->args = args;
    this->states = states;
}

Instruction::~Instruction() {

}

void Instruction::delete_states() {
    for (State* state : states) {
        delete state;
    }
}

InstructionType Instruction::get_type() {
    return type;
}

std::vector<uint> Instruction::get_args() {
    return args;
}

std::vector<State*> Instruction::get_states() {
    return states;
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
        case InstructionType::BACKWARD:
            stream << "BACKWARD";
            break;
        case InstructionType::FORWARD:
            stream << "FORWARD";
            break;
        case InstructionType::ARG:
            stream << "ARG";
            break;
        case InstructionType::RETURN:
            stream << "RETURN";
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
        case InstructionType::FUNCTION:
            stream << "FUNCTION";
            break;
        case InstructionType::COMPARE:
            stream << "COMPARE";
            break;
        case InstructionType::NOT_EQUALS:
            stream << "NOT_EQUALS";
            break;
        default:
            throw Exception("No name for instruction type");
    }
    for (int j = 0; j < get_args().size(); j++) {
        int arg = get_args()[j];
        stream << " " << arg;
    }
    stream << " ";
    for (State* state : states) {
        stream << "{" << (uint)state->get_type().get_main_type() << " " << state->get_displayable_string() << "}";
    }
    stream << std::endl;
    return stream.str();
}

std::string Instruction::get_string() {
    return "";
}

std::vector<State*> Instruction::get_state_cache() {
    return state_cache;
}

void Instruction::set_state_cache(std::vector<State*> state_cache) {
    this->state_cache = state_cache;
}