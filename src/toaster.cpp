#include <iostream>
#include <fstream>
#include "toast.h"
using namespace toast;

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


    // std::vector<Instruction> instructions;
    // instructions.push_back(Instruction(PUSH, { INT }));
    // instructions.push_back(Instruction(SET, { -1, 0, 10 }));
    // instructions.push_back(Instruction(ADD, { -1, 0, -1, 0, INT, 35 }));
    // instructions.push_back(Instruction(PUSH, { std::string }));
    // instructions.push_back(Instruction(SET, { -1, 0 }, "Hello world!"));
    // instructions.push_back(Instruction(PUSH, { FUNC, 1, BOOL, 1, INT }));
    // instructions.push_back(Instruction(SET, { -1, 0 }));
    // instructions.push_back(Instruction(SKIP, { 6 }));
    // instructions.push_back(Instruction(PUSH, { INT }));
    // instructions.push_back(Instruction(ARG, { -2, 0 }));
    // instructions.push_back(Instruction(RETURN, { BOOL, 1 }));
    // instructions.push_back(Instruction(POP, {  }));
    // instructions.push_back(Instruction(BACK, {  }));
    // instructions.push_back(Instruction(EXIT, {  }));
    // instructions.push_back(Instruction(FRAME, { 1 }));
    // instructions.push_back(Instruction(CALL, { -1, 0, INT, 20 }));
    // instructions.push_back(Instruction(PUSH, { BOOL }));
    // instructions.push_back(Instruction(PUSH, { BOOL }));
    // instructions.push_back(Instruction(MOVE_RETURN, { -1, 0 }));
    // instructions.push_back(Instruction(EQUALS, { -1, 1, -1, 0, BOOL, 0 }));
    // instructions.push_back(Instruction(POP, {  }));
    // instructions.push_back(Instruction(IF, { -1, 0 }));
    // instructions.push_back(Instruction(SKIP, { 4 }));
    // instructions.push_back(Instruction(PUSH, { INT }));
    // instructions.push_back(Instruction(SET, { -1, 0, 30 }));
    // instructions.push_back(Instruction(POP, {  }));
    // instructions.push_back(Instruction(SKIP, { 3 }));
    // instructions.push_back(Instruction(PUSH, { INT }));
    // instructions.push_back(Instruction(SET, { -1, 0, 25 }));
    // instructions.push_back(Instruction(POP, {  }));
    // instructions.push_back(Instruction(POP, {  }));
    // instructions.push_back(Instruction(PUSH, { ARRAY, 1, INT }));
    // instructions.push_back(Instruction(SET, { -1, 0 }));
    // instructions.push_back(Instruction(STREAM_IN, { -1, 0, INT, 10 }));
    // instructions.push_back(Instruction(PUSH, { INT }));
    // instructions.push_back(Instruction(STREAM_IN, { -1, 0, -1, 1 }));
    // instructions.push_back(Instruction(PUSH, { INT }));
    // instructions.push_back(Instruction(LENGTH, { -1, 0, -1, 2 }));
    // instructions.push_back(Instruction(STREAM_OUT, { -1, 2 }));
    // instructions.push_back(Instruction(DELETE, { -1, 2 }));
    // instructions.push_back(Instruction(PUSH, { std::string }));
    // instructions.push_back(Instruction(SET, { -1, 0 }, " Goodbye world!"));
    // instructions.push_back(Instruction(STREAM_IN, { -1, 5, -1, 0 }));
    // instructions.push_back(Instruction(POP, {  }));
    // instructions.push_back(Instruction(POP, {  }));
    // instructions.push_back(Instruction(POP, {  }));
    // instructions.push_back(Instruction(POP, {  }));
    // instructions.push_back(Instruction(POP, {  }));
    // instructions.push_back(Instruction(POP, {  }));
    // instructions.push_back(Instruction(POP, {  }));
    // execute(instructions);
    return 0;
}