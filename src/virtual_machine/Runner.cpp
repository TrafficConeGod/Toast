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
            StateFunction function = from->get_value<StateFunction>();
            position = function.get_position();
            frames.push_back(function.get_frame().clone_ptr());
            return_state = into;
        } break;
        case InstructionType::RETURN: {
            std::vector<State*> states = get_states(instruction);
            State* from = states[0];
            return_state->move_value_from(from);
        } break;
        case InstructionType::EXIT: {
            delete frames.back();
            frames.pop_back();
            position = return_stack.back();
            return_stack.pop_back();
        } break;
    }
    // switch (type) {
    //     case InstructionType::PUSH: {
    //         StateTypeHolder type(args);
    //         State* state = push_state(type);
    //     } break;
    //     case InstructionType::POP: {
    //         delete pop_state();
    //     } break;
    //     case InstructionType::SET: {
    //         State* state = get_state(args[0], args[1]);
    //         switch (state->get_type().get_main_type()) {
    //             case StateType::INT: {
    //                 state->set_value<int>(args[2]);
    //             } break;
    //             case StateType::BOOL: {
    //                 state->set_value<bool>(args[2]);
    //             } break;
    //             case StateType::FUNC: {
    //                 state->set_value<int>(position + 1);
    //             } break;
    //             case StateType::STRING: {
    //                 state->set_value<std::string>(instruction.get_string());
    //             } break;
    //             case StateType::ARRAY: {
    //                 StateArray* array = new StateArray();
    //                 state->set_value<StateArray*>(array);
    //             } break;
    //             default: {
    //                 throw Exception("No support for type");
    //             } break;
    //         }
    //         // state->clean();
    //     } break;
    //     case InstructionType::SKIP: {
    //         position += args[0];
    //     } break;
    //     case InstructionType::MOVE: {
    //         State* state = get_state(args[0], args[1]);
    //         State* from = get_state(args[2], args[3]);
    //         state->move_value_from(from);
    //         state->clean();
    //         from->clean();
    //     } break;
    //     case InstructionType::CALL: {
    //         State* state = get_state(args[0], args[1]);
    //         return_stack.push_back(position);
    //         position = state->get_value<int>();
    //         for (int i = 2; i < args.size(); i += 2) {
    //             State* arg_state = get_state(args[i], args[i + 1]);
    //             call_args.push_back(arg_state);
    //         }
    //         state->clean();
    //     } break;
    //     case InstructionType::EXIT: {
    //         position = return_stack.back();
    //         return_stack.pop_back();
    //     } break;
    //     case InstructionType::FRAME: {
    //         int frame_key = args[0];
    //         set_frame(frame_key, true);
    //     } break;
    //     case InstructionType::BACK: {
    //         Frame* frame = frames[frame_key];
    //         frames.erase(frame_key);
    //         set_frame(frame->get_return(), false);
    //         delete frame;
    //     } break;
    //     case InstructionType::ARG: {
    //         State* move_into = get_state(args[0], args[1]);
    //         State* arg_state = call_args.front();
    //         call_args.pop_front();
    //         move_into->move_value_from(arg_state);
    //         arg_state->clean();
    //     } break;
    //     case InstructionType::RETURN: {
    //         return_state = get_state(args[0], args[1]);
    //         // later add skipping code stuff
    //     } break;
    //     case InstructionType::MOVE_RETURN: {
    //         State* move_to = get_state(args[0], args[1]);
    //         move_to->move_value_from(return_state);
    //         return_state->clean();
    //         move_to->clean();
    //     } break;
    //     case InstructionType::EQUALS: {
    //         State* move_into = get_state(args[0], args[1]);
    //         State* equals_1 = get_state(args[2], args[3]);
    //         State* equals_2 = get_state(args[4], args[5]);
    //         move_into->set_value<bool>(equals_1->equals(equals_2));
    //         equals_1->clean();
    //         equals_2->clean();
    //     } break;
    //     case InstructionType::ADD:
    //     case InstructionType::SUBTRACT:
    //     case InstructionType::MULTIPLY:
    //     case InstructionType::DIVIDE: {
    //         State* move_into = get_state(args[0], args[1]);
    //         State* op_1_state = get_state(args[2], args[3]);
    //         State* op_2_state = get_state(args[4], args[5]);
    //         int val = op_1_state->get_value<int>();
    //         int val_2 = op_2_state->get_value<int>();
    //         switch (type) {
    //             case InstructionType::ADD: val += val_2; break;
    //             case InstructionType::SUBTRACT: val -= val_2; break;
    //             case InstructionType::MULTIPLY: val *= val_2; break;
    //             case InstructionType::DIVIDE: val /= val_2; break;
    //         }
    //         move_into->set_value<int>(val);
    //     } break;
    //     case InstructionType::IF: {
    //         State* state = get_state(args[0], args[1]);
    //         bool val = state->get_value<bool>();
    //         if (val) {
    //             position++;
    //         }
    //         state->clean();
    //     } break;
    //     case InstructionType::LENGTH: {
    //         State* move_into = get_state(args[0], args[1]);
    //         State* state = get_state(args[2], args[3]);
    //         switch (state->get_type().get_main_type()) {
    //             case StateType::STRING: {
    //                 std::string string = state->get_value<std::string>();
    //                 move_into->set_value<int>(string.size());
    //             } break;
    //             case StateType::ARRAY: {
    //                 StateArray* array = state->get_value<StateArray*>();
    //                 move_into->set_value<int>(array->get_length());
    //             } break;
    //         }
    //         state->clean();
    //     } break;
    //     case InstructionType::STREAM_IN: {
    //         State* stream_into = get_state(args[0], args[1]);
    //         State* stream_from = get_state(args[2], args[3]);
    //         switch (stream_into->get_type().get_main_type()) {
    //             case StateType::STRING: {
    //                 std::stringstream stream;
    //                 std::string into = stream_into->get_value<std::string>();
    //                 std::string from = stream_from->get_value<std::string>();
    //                 stream << into;
    //                 stream << from;
    //                 stream_into->set_value<std::string>(stream.str());
    //             } break;
    //             case StateType::ARRAY: {
    //                 StateArray* array = stream_into->get_value<StateArray*>();
    //                 // make a clone because if the state gets popped out of scope bad things will happen
    //                 State* state_clone = stream_from->clone();
    //                 array->push_state(state_clone);
    //             } break;
    //             default: {
    //                 StateArray* array = stream_from->get_value<StateArray*>();
    //                 stream_into->move_value_from(array->get_back());
    //             } break;
    //         }
    //         stream_from->clean();
    //     } break;
    //     case InstructionType::STREAM_OUT: {
    //         State* stream_from = get_state(args[0], args[1]);
    //         bool has_stream_into = false;
    //         State* stream_into;
    //         if (args.size() > 2) {
    //             has_stream_into = true;
    //             stream_into = get_state(args[2], args[3]);
    //         }
    //         StateArray* array = stream_from->get_value<StateArray*>();
    //         State* state = array->pop_state();
    //         if (has_stream_into) {
    //             stream_into->move_value_from(state);
    //         }
    //         delete state;
    //     } break;
    //     case InstructionType::DELETE: {
    //         State* state = get_state(args[0], args[1]);
    //         switch (state->get_type().get_main_type()) {
    //             case StateType::ARRAY: {
    //                 StateArray* array = state->get_value<StateArray*>();
    //                 delete array;
    //             } break;
    //         }
    //     } break;
    //     default: {
    //         throw Exception("No support for instruction");
    //     } break;
    // }
}