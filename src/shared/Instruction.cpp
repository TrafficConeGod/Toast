#include "Instruction.h"
using namespace toast;
using namespace std;

Instruction::Instruction(InstructionType type, vector<int> args) {
    this->type = type;
    this->args = args;
}

InstructionType Instruction::get_type() {
    return type;
}

vector<int> Instruction::get_args() {
    return args;
}

string Instruction::make_human_readable() {
    stringstream stream;
    switch (get_type()) {
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
        case ARG:
            stream << "ARG";
            break;
        case RETURN:
            stream << "RETURN";
            break;
        case MOVE_RETURN:
            stream << "MOVE_RETURN";
            break;
        case ADD:
            stream << "ADD";
            break;
        case SUBTRACT:
            stream << "SUBTRACT";
            break;
        case MULTIPLY:
            stream << "MULTIPLY";
            break;
        case DIVIDE:
            stream << "DIVIDE";
            break;
        case EQUALS:
            stream << "EQUALS";
            break;
        case IF:
            stream << "IF";
            break;
        case LENGTH:
            stream << "LENGTH";
            break;
        case STREAM_IN:
            stream << "STREAM_IN";
            break;
        case STREAM_OUT:
            stream << "STREAM_OUT";
            break;
        case MOVE_IN:
            stream << "MOVE_IN";
            break;
        case MOVE_OUT:
            stream << "MOVE_OUT";
            break;
        case DELETE:
            stream << "DELETE";
            break;
        default:
            throw Exception("No name for instruction type");
    }
    for (int j = 0; j < get_args().size(); j++) {
        int arg = get_args()[j];
        stream << " " << arg;
    }
    stream << endl;
    return stream.str();
}

Instruction::Instruction(InstructionType type, vector<int> args, string str) {
    this->type = type;
    this->args = args;
    this->str = str;
}

string Instruction::get_string() {
    return str;
}
