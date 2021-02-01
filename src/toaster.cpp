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
    instructions.push_back(toast::Instruction(toast::SET, { -1, 0, 10 }));
    instructions.push_back(toast::Instruction(toast::ADD, { -1, 0, -1, 0, toast::INT, 35 }));
    instructions.push_back(toast::Instruction(toast::PUSH, { toast::STRING }));
    instructions.push_back(toast::Instruction(toast::SET, { -1, 0 }, "Hello world!"));
    instructions.push_back(toast::Instruction(toast::PUSH, { toast::FUNC, 1, toast::BOOL, 1, toast::INT }));
    instructions.push_back(toast::Instruction(toast::SET, { -1, 0 }));
    instructions.push_back(toast::Instruction(toast::SKIP, { 6 }));
    instructions.push_back(toast::Instruction(toast::PUSH, { toast::INT }));
    instructions.push_back(toast::Instruction(toast::ARG, { -2, 0 }));
    instructions.push_back(toast::Instruction(toast::RETURN, { toast::BOOL, 1 }));
    instructions.push_back(toast::Instruction(toast::POP, {  }));
    instructions.push_back(toast::Instruction(toast::BACK, {  }));
    instructions.push_back(toast::Instruction(toast::EXIT, {  }));
    instructions.push_back(toast::Instruction(toast::FRAME, { 1 }));
    instructions.push_back(toast::Instruction(toast::CALL, { -1, 0, toast::INT, 20 }));
    instructions.push_back(toast::Instruction(toast::PUSH, { toast::BOOL }));
    instructions.push_back(toast::Instruction(toast::PUSH, { toast::BOOL }));
    instructions.push_back(toast::Instruction(toast::MOVE_RETURN, { -1, 0 }));
    instructions.push_back(toast::Instruction(toast::EQUALS, { -1, 1, -1, 0, toast::BOOL, 0 }));
    instructions.push_back(toast::Instruction(toast::POP, {  }));
    instructions.push_back(toast::Instruction(toast::IF, { -1, 0 }));
    instructions.push_back(toast::Instruction(toast::SKIP, { 4 }));
    instructions.push_back(toast::Instruction(toast::PUSH, { toast::INT }));
    instructions.push_back(toast::Instruction(toast::SET, { -1, 0, 30 }));
    instructions.push_back(toast::Instruction(toast::POP, {  }));
    instructions.push_back(toast::Instruction(toast::SKIP, { 3 }));
    instructions.push_back(toast::Instruction(toast::PUSH, { toast::INT }));
    instructions.push_back(toast::Instruction(toast::SET, { -1, 0, 25 }));
    instructions.push_back(toast::Instruction(toast::POP, {  }));
    instructions.push_back(toast::Instruction(toast::POP, {  }));
    instructions.push_back(toast::Instruction(toast::PUSH, { toast::ARRAY, 1, toast::INT }));
    instructions.push_back(toast::Instruction(toast::STREAM_IN, { -1, 0, toast::INT, 10 }));
    instructions.push_back(toast::Instruction(toast::PUSH, { toast::INT }));
    instructions.push_back(toast::Instruction(toast::STREAM_IN, { -1, 0, -1, 1 }));
    instructions.push_back(toast::Instruction(toast::PUSH, { toast::INT }));
    instructions.push_back(toast::Instruction(toast::LENGTH, { -1, 0, -1, 2 }));
    instructions.push_back(toast::Instruction(toast::STREAM_OUT, { -1, 2 }));
    instructions.push_back(toast::Instruction(toast::DELETE, { -1, 2 }));
    instructions.push_back(toast::Instruction(toast::POP, {  }));
    instructions.push_back(toast::Instruction(toast::POP, {  }));
    instructions.push_back(toast::Instruction(toast::POP, {  }));
    instructions.push_back(toast::Instruction(toast::POP, {  }));
    instructions.push_back(toast::Instruction(toast::POP, {  }));
    instructions.push_back(toast::Instruction(toast::POP, {  }));
    t_vm::execute(instructions);
    return 0;
}