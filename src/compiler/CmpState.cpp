#include "CmpState.h"
#include "t_cmp.h"
using namespace toast;
using namespace std;
using State = CmpState;

State::CmpState(StateTypeHolder type, int stack_frame) {
    this->type.push_back(type);
    this->stack_frame = stack_frame;
}

StateTypeHolder State::get_type() {
    return type.back();
}

int State::get_frame() {
    return stack_frame;
}

void State::set_offset(int offset) {
    this->offset = offset;
}

int State::get_offset() {
    return offset;
}

std::vector<int> State::get_args() {
    return { frame_negate(stack_frame), offset };
}