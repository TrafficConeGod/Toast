#pragma once
#include "../shared/Instruction.h"
#include "../shared/StateTypeHolder.h"
#include "CompilerException.h"
#include "Token.h"
#include "Lexer.h"
#include "Parser.h"
#include "Builder.h"
#include <iostream>
#include <vector>
#include <deque>
#include <any>

namespace toast {
    std::vector<Instruction> generate_instruction_list(std::string source);
    std::string make_human_readable(std::vector<Instruction>);
    std::any parse_val(std::string literal, StateTypeHolder type);
    void expected(std::string expected, std::string actual);
    void already_declared(std::string name);
    void not_declared(std::string name);
    std::vector<int> offset_args(std::vector<int> args, int amount);
    int frame_negate(int frame_key);
    template<typename T>
    void merge(std::vector<T>* first, std::vector<T> second) {
        first->insert(first->end(), second.begin(), second.end());
    }
}