#pragma once
#include "../shared/Instruction.h"
#include "Runner.h"
#include <iostream>

namespace toast {
    void execute(std::vector<Instruction> instructions);
}