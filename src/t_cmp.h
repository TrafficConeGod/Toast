#pragma once
#include <iostream>
#include <vector>
#include <map>
#include "toast.h"

namespace t_cmp {

    std::vector<toast::Instruction> generate_instruction_list(std::string source);
    std::string make_human_readable(std::vector<toast::Instruction>);
    int parse_val(std::string literal, toast::StateTypeHolder type);
    void delete_instruction_list(std::vector<toast::Instruction> instructions);
}
