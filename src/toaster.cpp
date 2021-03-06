#include <iostream>
#include <fstream>
#include "toast.h"
using namespace toast;
using i = InstructionType;
using h = StateTypeHolder;
using s = StateType;

std::string read_file(std::string path) {
    std::ifstream in_file(path);

    std::string str;
    in_file.seekg(0, std::ios::end);   
    str.reserve(in_file.tellg());
    in_file.seekg(0, std::ios::beg);

    str.assign((std::istreambuf_iterator<char>(in_file)),
                std::istreambuf_iterator<char>());
    return str;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        throw Exception("Not enough args");
    }
    char* path = argv[1];
    std::string source = read_file(path);
    std::vector<Instruction> instructions = generate_instruction_list(source);
    std::string toasted = make_human_readable(instructions);
    std::cout << toasted << std::endl;
    execute(instructions);


    // std::vector<Instruction> instructions = {
    //     // int a = 2
    //     Instruction(i::PUSH, { 1 }),
    //     Instruction(i::MOVE, { 1, 0 }, { new State(h(s::INT), 2) }),
    //     // int b = a + 3
    //     Instruction(i::PUSH, { 2 }),
    //     Instruction(i::PUSH, { 3 }),
    //     Instruction(i::ADD, { 3, 1, 0 }, { new State(h(s::INT), 3) }),
    //     Instruction(i::MOVE, { 2, 3 }),
    //     Instruction(i::POP, { 3 }),
    //     // int my_func() {
    //     Instruction(i::PUSH, { 4 }),
    //     Instruction(i::FUNCTION, { 4, 0 }, { new State(h(s::FUNC), 0) }),
    //     Instruction(i::FORWARD, { 11 }),
    //     // int c = b + 100
    //     Instruction(i::PUSH, { 5 }),
    //     Instruction(i::PUSH, { 6 }),
    //     Instruction(i::ADD, { 6, 2, 0 }, { new State(h(s::INT), 100) }),
    //     Instruction(i::MOVE, { 5, 6 }),
    //     Instruction(i::POP, { 6 }),
    //     // c++
    //     Instruction(i::ADD, { 5, 5, 0 }, { new State(h(s::INT), 1) }),
    //     // return c
    //     Instruction(i::RETURN, { 5 }),
    //     Instruction(i::POP, { 5 }),
    //     Instruction(i::EXIT, {}),
    //     // }
    //     Instruction(i::POP, { 5 }),
    //     Instruction(i::EXIT, {}),
    //     // int d = a
    //     Instruction(i::PUSH, { 7 }),
    //     Instruction(i::MOVE, { 7, 1 }),
    //     // my_func()
    //     Instruction(i::PUSH, { 8 }),
    //     Instruction(i::CALL, { 8, 4 }),
    //     Instruction(i::POP, { 8 }),
    //     // if (a == 2) {
    //     Instruction(i::PUSH, { 9 }),
    //     Instruction(i::EQUALS, { 9, 1, 0 }, { new State(h(s::INT), 2) }),
    //     Instruction(i::COMPARE, { 9 }),
    //     Instruction(i::POP, { 9 }),
    //     Instruction(i::IF, { }),
    //     Instruction(i::FORWARD, { 3 }),
    //     // int k = 10
    //     Instruction(i::PUSH, { 10 }),
    //     Instruction(i::MOVE, { 10, 0 }, { new State(h(s::INT), 10) }),
    //     Instruction(i::POP, { 10 }),
    //     // }
    //     // exit scope
    //     Instruction(i::POP, { 1 }),
    //     Instruction(i::POP, { 2 }),
    //     Instruction(i::POP, { 4 }),
    //     Instruction(i::POP, { 7 }),
    // };
    // toast::execute(instructions);
    // for (Instruction in : instructions) {
    //     in.delete_states();
    // }
    return 0;
}