#include <iostream>
#include <fstream>
#include "t_vm.h"
#include "t_cmp.h"

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
    // if (argc < 2) {
        // throw toast::Exception("Not enough args");
    // }
    // char* path = argv[1];
    // std::string source = read_file(path);
    // std::vector<toast::Instruction> instructions = t_cmp::generate_instruction_list(source);
    // std::string toasted = t_cmp::make_human_readable(instructions);
    // std::cout << toasted << std::endl;
    // t_vm::execute(instructions);
    // t_cmp::delete_instruction_list(instructions);
    std::vector<toast::Instruction> instructions;
    instructions.push_back(toast::Instruction(toast::PUSH, { toast::INT }));
    instructions.push_back(toast::Instruction(toast::SET, { 0, 0, 10 }));
    instructions.push_back(toast::Instruction(toast::PUSH, { toast::STRING }));
    instructions.push_back(toast::Instruction(toast::SET, { 0, 0 }, "Hello world!"));
    instructions.push_back(toast::Instruction(toast::PUSH, { toast::FUNC, 1, toast::BOOL, 1, toast::INT }));
    instructions.push_back(toast::Instruction(toast::SET, { 0, 0 }));
    instructions.push_back(toast::Instruction(toast::SKIP, { 9 }));
    instructions.push_back(toast::Instruction(toast::PUSH, { toast::INT }));
    instructions.push_back(toast::Instruction(toast::ARG, { 1, 0 }));
    instructions.push_back(toast::Instruction(toast::PUSH, { toast::BOOL }));
    instructions.push_back(toast::Instruction(toast::SET, { 1, 0, 1 }));
    instructions.push_back(toast::Instruction(toast::RETURN, { 1, 0 }));
    instructions.push_back(toast::Instruction(toast::POP, {  }));
    instructions.push_back(toast::Instruction(toast::POP, {  }));
    instructions.push_back(toast::Instruction(toast::BACK, {  }));
    instructions.push_back(toast::Instruction(toast::EXIT, {  }));
    instructions.push_back(toast::Instruction(toast::PUSH, { toast::INT }));
    instructions.push_back(toast::Instruction(toast::SET, { 0, 0, 20 }));
    instructions.push_back(toast::Instruction(toast::FRAME, { 1 }));
    instructions.push_back(toast::Instruction(toast::CALL, { 0, 1, 0, 0 }));
    instructions.push_back(toast::Instruction(toast::POP, {  }));
    instructions.push_back(toast::Instruction(toast::PUSH, { toast::BOOL }));
    instructions.push_back(toast::Instruction(toast::MOVE_RETURN, { 0, 0 }));
    instructions.push_back(toast::Instruction(toast::PUSH, { toast::BOOL }));
    instructions.push_back(toast::Instruction(toast::SET, { 0, 0, 0 }));
    t_vm::execute(instructions);
    return 0;
}