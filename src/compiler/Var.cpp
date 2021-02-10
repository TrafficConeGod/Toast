#include "Var.h"
#include "t_cmp.h"
using namespace toast;

Var::Var(StateTypeHolder type, int stack_frame) {
    this->type.push_back(type);
    this->stack_frame = stack_frame;
}

StateTypeHolder Var::get_type() {
    return type.back();
}

int Var::get_frame() {
    return stack_frame;
}

void Var::set_offset(int offset) {
    this->offset = offset;
}

int Var::get_offset() {
    return offset;
}

std::vector<int> Var::get_args() {
    return { frame_negate(stack_frame), offset };
}