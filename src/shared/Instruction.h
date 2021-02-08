#pragma once
#include <iostream>
#include <vector>
#include <sstream>
#include "InstructionType.h"
#include "Exception.h"

namespace toast {
    class Instruction {
        private:
            InstructionType type;
            std::vector<int> args;
            std::string str;
        public:
            Instruction(InstructionType type, std::vector<int> args);
            Instruction(InstructionType type, std::vector<int> args, std::string str);
            InstructionType get_type();
            std::vector<int> get_args();
            std::string get_string();
            std::string make_human_readable();
    };
}