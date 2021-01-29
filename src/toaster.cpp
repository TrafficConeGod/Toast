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
    if (argc < 2) {
        throw toast::Exception("Not enough args");
    }
    char* path = argv[1];
    std::string source = read_file(path);
    std::vector<toast::Instruction> instructions = t_cmp::generate_instruction_list(source);
    std::string toasted = t_cmp::make_human_readable(instructions);
    std::cout << toasted << std::endl;
    // t_vm::execute(instructions);
    t_cmp::delete_instruction_list(instructions);
    return 0;
}