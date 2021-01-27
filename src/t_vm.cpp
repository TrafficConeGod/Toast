#include "t_vm.h"

void t_vm::execute(std::vector<toast::Instruction*> instructions) {
    Runner runner = Runner(instructions);
}

t_vm::Runner::Runner(std::vector<toast::Instruction*> instructions) {
    set_frame(0);
    this->instructions = instructions;
    for (position = 0; position < instructions.size(); position++) {
        handle_instruction();
    }
}

t_vm::Runner::~Runner() {

}

void t_vm::Runner::set_frame(int key) {
    if (key == frame_key) {
        return;
    }
    if (frames.count(key) == 0) {
        frames[key] = new Frame();
    }
    Frame* frame = frames[key];
    frame->push_stack(frame_key);
    frame_key = key;
}

t_vm::State* t_vm::Runner::get_state(int key, int offset) {
    if (frames.count(key) == 0) {
        throw toast::Exception("Frame does not exist");
    }
    Frame* frame = frames[key];
    return frame->get_state(offset);
}

t_vm::State* t_vm::Runner::push_state(toast::StateTypeHolder* type) {
    State* state = new State(type);
    Frame* frame = frames[frame_key];
    frame->push_state(state);
    return state;
}

t_vm::State* t_vm::Runner::pop_state() {
    Frame* frame = frames[frame_key];
    State* state = frame->pop_state();
    std::cout << "Popped state of type " << state->get_type()->get_main_type() << " of value {";
    for (int val : state->get_value()) {
        std::cout << " " << val;
    } 
    std::cout << " } from the stack" << std::endl;
    return state;
}

void t_vm::Runner::handle_instruction() {
    toast::Instruction* instruction = instructions[position];
    toast::InstructionType type = instruction->get_type();
    std::vector<int> args = instruction->get_args();
    switch (type) {
        case toast::PUSH: {
            int type_id = args[0];
            toast::StateType type_enum = (toast::StateType) type_id;
            toast::StateTypeHolder* type = new toast::StateTypeHolder(type_enum);
            push_state(type);
        } break;
        case toast::POP: {
            pop_state();
        } break;
        case toast::SET: {
            // State* state = get_state(args[0], args[1]);

        } break;
        case toast::BACK: {
            Frame* frame = frames[frame_key];
            frames.erase(frame_key);
            set_frame(frame->get_return());
            delete frame;
        } break;
    }
}

t_vm::State::State(toast::StateTypeHolder* type) {
    this->type = type;
}

void t_vm::Frame::push_stack(int key) {
    keys.push_back(key);
    stacks.push_back( new Stack() );
}

void t_vm::Frame::pop_stack() {
    last = keys.back();
    keys.pop_back();
    delete stacks.back();
    stacks.pop_back();
}

t_vm::State* t_vm::Frame::get_state(int offset) {
    Stack* stack = stacks.back();
    return stack->get_state(offset);
}

void t_vm::Frame::push_state(State* state) {
    Stack* stack = stacks.back();
    stack->push_state(state);
}

t_vm::State* t_vm::Frame::pop_state() {
    Stack* stack = stacks.back();
    State* state = stack->pop_state();
    return state;
}

int t_vm::Frame::get_return() {
    return last;
}

toast::StateTypeHolder* t_vm::State::get_type() {
    return type;
}

std::vector<int> t_vm::State::get_value() {
    return value;
}



t_vm::State* t_vm::Stack::get_state(int offset) {
    State* state = states[(states.size() - 1) - offset];
    return state;
}

void t_vm::Stack::push_state(State* state) {
    states.push_back(state);
}

t_vm::State* t_vm::Stack::pop_state() {
    State* state = states.back();
    states.pop_back();
    return state;
}

t_vm::Frame::~Frame() {}
t_vm::Stack::~Stack() {}
t_vm::State::~State() {}