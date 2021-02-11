#include "StateFunction.h"
using namespace toast;

StateFunction::StateFunction(uint position, Frame* frame) {
    this->position = position;
    this->frame = frame;
}

StateFunction::~StateFunction() {

}

uint StateFunction::get_position() {
    return position;
}

Frame* StateFunction::get_frame() {
    return frame;
}