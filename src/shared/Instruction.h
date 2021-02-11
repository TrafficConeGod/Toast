#pragma once
#include <iostream>
#include <vector>
#include <sstream>
#include "InstructionType.h"
#include "Exception.h"
#include "State.h"

namespace toast {
    class Instruction {
        private:
            InstructionType type;
            std::vector<uint> args;
            std::vector<State*> states;
        public:
            Instruction(InstructionType type, std::vector<uint> args);
            Instruction(InstructionType type, std::vector<uint> args, std::vector<State*> states);
            ~Instruction();
            InstructionType get_type();
            std::vector<uint> get_args();
            std::vector<State*> get_states();
            std::string get_string();
            std::string make_human_readable();
    };
}