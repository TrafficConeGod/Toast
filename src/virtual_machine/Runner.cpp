#include "Runner.h"
#include "StateFunction.h"
using namespace toast;

Runner::Runner(std::vector<Instruction> instructions) {
    // set_frame(0, true);
    frames.push_back(new Frame());
    this->instructions = instructions;
    for (position = 0; position < instructions.size(); position++) {
        handle_instruction();
    }
}

Runner::~Runner() {
    for (Frame* frame : frames) {
        frame->delete_states();
        delete frame;
    }
    delete compare_state;
    // std::map<int, Frame*>::iterator it;
    // for (it = frames.begin(); it != frames.end(); it++) {
    //     delete it->second;
    // }
    // for (State* state : call_args) {
    //     delete state;
    // }
    // delete return_state;
}

// void Runner::set_frame(int key, bool push_stack) {
//     // if (key == frame_key) {
//     //     return;
//     // }
//     if (frames.count(key) == 0) {
//         frames[key] = new Frame();
//     }
//     Frame* frame = frames[key];
//     if (push_stack) {
//         frame->push_stack(frame_key);
//     }
//     frame_key = key;
// }

// State* Runner::get_state(int val, int offset) {
//     if (val >= 0) {
//         StateTypeHolder type((StateType) val);
//         State* state = new State(type);
//         state->set_temp(true);
//         switch (state->get_type().get_main_type()) {
//             case StateType::INT: {
//                 state->set_value<int>(offset);
//             } break;
//             case StateType::BOOL: {
//                 state->set_value<bool>(offset);
//             } break;
//             default: {
//                 throw Exception("No support for quick type");
//             } break;
//         }
//         return state;
//     } else {
//         int key = (val + 1) * -1;
//         if (frames.count(key) == 0) {
//             throw Exception("Frame does not exist");
//         }
//         Frame* frame = frames[key];
//         return frame->get_state(offset);
//     }
// }

void Runner::push_state(uint state_key) {
    Frame* frame = frames.back();
    frame->push_state(state_key);
}

State* Runner::pop_state(uint state_key) {
    Frame* frame = frames.back();
    State* state = frame->pop_state(state_key);
    std::cout << "Popped state";
    if (!state->is_empty()) {
        std::cout << " of type " << (int)state->get_type().get_main_type();
        std::cout << " and value " << state->get_displayable_string();
    }
    std::cout << std::endl;
    return state;
}

State* Runner::get_state(uint state_key) {
    Frame* frame = frames.back();
    return frame->get_state(state_key);
}

std::vector<State*> Runner::get_states(Instruction instruction) {
    std::vector<State*> return_states;
    std::vector<uint> args = instruction.get_args();
    std::vector<State*> states = instruction.get_states();
    for (int i = 0, j = 0; i < args.size(); i++) {
        int arg = args[i];
        if (arg == 0) {
            return_states.push_back(states[j]);
            j++;
        } else {
            return_states.push_back(get_state(arg));
        }
    }
    return return_states;
}

void Runner::handle_instruction() {
    Instruction instruction = instructions[position];
    InstructionType type = instruction.get_type();
    std::vector<uint> args = instruction.get_args();
    std::cout << instruction.make_human_readable();
    switch (type) {
        case InstructionType::PUSH: {
            int state_key = args[0];
            push_state(state_key);
        } break;
        case InstructionType::POP: {
            int state_key = args[0];
            State* state = pop_state(state_key);
            delete state;
        } break;
        case InstructionType::MOVE: {
            std::vector<State*> states = get_states(instruction);
            State* into = states[0];
            State* from = states[1];
            into->move_value_from(from);
        } break;
        case InstructionType::ADD:
        case InstructionType::SUBTRACT:
        case InstructionType::MULTIPLY:
        case InstructionType::DIVIDE: {
            std::vector<State*> states = get_states(instruction);
            State* into = states[0];
            State* op_1 = states[1];
            State* op_2 = states[2];
            int val = op_1->get_value<int>();
            int val_2 = op_2->get_value<int>();
            switch (type) {
                case InstructionType::ADD: val += val_2; break;
                case InstructionType::SUBTRACT: val -= val_2; break;
                case InstructionType::MULTIPLY: val *= val_2; break;
                case InstructionType::DIVIDE: val /= val_2; break;
            }
            into->set_type(op_1->get_type());
            into->set_value<int>(val);
        } break;
        case InstructionType::FORWARD: {
            position += args[0];
        } break;
        case InstructionType::BACKWARD: {
            position -= args[0];
        } break;
        case InstructionType::EQUALS:
        case InstructionType::NOT_EQUALS: {
            std::vector<State*> states = get_states(instruction);
            State* into = states[0];
            State* op_1 = states[1];
            State* op_2 = states[2];
            into->set_type(StateTypeHolder(StateType::BOOL));
            if (type == InstructionType::EQUALS) {
                into->set_value<bool>(op_1->equals(op_2));
            } else {
                into->set_value<bool>(!(op_1->equals(op_2)));
            }
        } break;
        case InstructionType::COMPARE: {
            std::vector<State*> states = get_states(instruction);
            State* from = states[0];
            compare_state->move_value_from(from);
        } break;
        case InstructionType::IF: {
            if (compare_state->get_value<bool>()) {
                position++;
            }
        } break;
        case InstructionType::FUNCTION: {
            std::vector<State*> states = get_states(instruction);
            State* into = states[0];
            State* type_state = states[1];
            into->move_value_from(type_state);
            Frame clone = frames.back()->clone();
            into->set_value<StateFunction>(StateFunction(position + 1, clone));
        } break;
        case InstructionType::CALL: {
            return_stack.push_back(position);
            std::vector<State*> states = get_states(instruction);
            State* into = states[0];
            State* from = states[1];
            for (int i = 2; i < states.size(); i++) {
                call_args.push_back(states[i]);
            }
            StateFunction function = from->get_value<StateFunction>();
            position = function.get_position();
            frames.push_back(function.get_frame().clone_ptr());
            return_states.push_back(into);
        } break;
        case InstructionType::ARG: {
            std::vector<State*> states = get_states(instruction);
            State* into = states[0];
            into->move_value_from(call_args.front());
            call_args.pop_front();
        } break;
        case InstructionType::RETURN: {
            std::vector<State*> states = get_states(instruction);
            State* from = states[0];
            return_states.back()->move_value_from(from);
            return_states.pop_back();
        } break;
        case InstructionType::EXIT: {
            delete frames.back();
            frames.pop_back();
            position = return_stack.back();
            return_stack.pop_back();
        } break;
    }
}