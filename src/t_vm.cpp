#include "t_vm.h"

void t_vm::execute(std::vector<toast::Instruction> instructions) {
    // try {
        Runner runner = Runner(instructions);
    // } catch (toast::Exception e) {
        // std::cout << e.what() << std::endl;
    // }
}

t_vm::Runner::Runner(std::vector<toast::Instruction> instructions) {
    set_frame(0, true);
    this->instructions = instructions;
    for (position = 0; position < instructions.size(); position++) {
        handle_instruction();
    }
}

t_vm::Runner::~Runner() {
    std::map<int, Frame*>::iterator it;
    for (it = frames.begin(); it != frames.end(); it++) {
        delete it->second;
    }
    for (State* state : call_args) {
        delete state;
    }
    // delete return_state;
}

void t_vm::Runner::set_frame(int key, bool push_stack) {
    // if (key == frame_key) {
    //     return;
    // }
    if (frames.count(key) == 0) {
        frames[key] = new Frame();
    }
    Frame* frame = frames[key];
    if (push_stack) {
        frame->push_stack(frame_key);
    }
    frame_key = key;
}

t_vm::State* t_vm::Runner::get_state(int val, int offset) {
    if (val >= 0) {
        toast::StateTypeHolder type((toast::StateType) val);
        State* state = new State(type);
        state->set_temp(true);
        switch (state->get_type().get_main_type()) {
            case toast::INT: {
                state->set_value<int>(offset);
            } break;
            case toast::BOOL: {
                state->set_value<bool>(offset);
            } break;
            default: {
                throw toast::Exception("No support for quick type");
            } break;
        }
        return state;
    } else {
        int key = (val + 1) * -1;
        if (frames.count(key) == 0) {
            throw toast::Exception("Frame does not exist");
        }
        Frame* frame = frames[key];
        return frame->get_state(offset);
    }
}

t_vm::State* t_vm::Runner::push_state(toast::StateTypeHolder type) {
    State* state = new State(type);
    Frame* frame = frames[frame_key];
    frame->push_state(state);
    return state;
}

t_vm::State* t_vm::Runner::pop_state() {
    Frame* frame = frames[frame_key];
    State* state = frame->pop_state();
    std::cout << "Popped state of type " << state->get_type().get_main_type();
    if (!state->is_empty()) {
        switch (state->get_type().get_main_type()) {
            case toast::INT:
                std::cout << " and value " << state->get_value<int>();
                break;
            case toast::BOOL:
                std::cout << " and value " << state->get_value<bool>();
                break;
            case toast::STRING:
                std::cout << " and value " << state->get_value<std::string>();
                break;
        }
    }
    std::cout << std::endl;
    return state;
}

void t_vm::Runner::handle_instruction() {
    toast::Instruction instruction = instructions[position];
    toast::InstructionType type = instruction.get_type();
    std::vector<int> args = instruction.get_args();
    std::cout << frame_key << " " << toast::make_human_readable(instruction);
    switch (type) {
        case toast::PUSH: {
            toast::StateTypeHolder type(args);
            State* state = push_state(type);
        } break;
        case toast::POP: {
            delete pop_state();
        } break;
        case toast::SET: {
            State* state = get_state(args[0], args[1]);
            switch (state->get_type().get_main_type()) {
                case toast::INT: {
                    state->set_value<int>(args[2]);
                } break;
                case toast::BOOL: {
                    state->set_value<bool>(args[2]);
                } break;
                case toast::FUNC: {
                    state->set_value<int>(position + 1);
                } break;
                case toast::STRING: {
                    state->set_value<std::string>(instruction.get_string());
                } break;
                case toast::ARRAY: {
                    StateArray* array = new StateArray();
                    state->set_value<StateArray*>(array);
                } break;
                default: {
                    throw toast::Exception("No support for type");
                } break;
            }
            // state->clean();
        } break;
        case toast::SKIP: {
            position += args[0];
        } break;
        case toast::MOVE: {
            State* state = get_state(args[0], args[1]);
            State* from = get_state(args[2], args[3]);
            state->move_value_from(from);
            state->clean();
            from->clean();
        } break;
        case toast::CALL: {
            State* state = get_state(args[0], args[1]);
            return_stack.push_back(position);
            position = state->get_value<int>();
            for (int i = 2; i < args.size(); i += 2) {
                State* arg_state = get_state(args[i], args[i + 1]);
                call_args.push_back(arg_state);
            }
            state->clean();
        } break;
        case toast::EXIT: {
            position = return_stack.back();
            return_stack.pop_back();
        } break;
        case toast::FRAME: {
            int frame_key = args[0];
            set_frame(frame_key, true);
        } break;
        case toast::BACK: {
            Frame* frame = frames[frame_key];
            frames.erase(frame_key);
            set_frame(frame->get_return(), false);
            delete frame;
        } break;
        case toast::ARG: {
            State* move_into = get_state(args[0], args[1]);
            State* arg_state = call_args.front();
            call_args.pop_front();
            move_into->move_value_from(arg_state);
            arg_state->clean();
        } break;
        case toast::RETURN: {
            return_state = get_state(args[0], args[1]);
            // later add skipping code stuff
        } break;
        case toast::MOVE_RETURN: {
            State* move_to = get_state(args[0], args[1]);
            move_to->move_value_from(return_state);
            return_state->clean();
            move_to->clean();
        } break;
        case toast::EQUALS: {
            State* move_into = get_state(args[0], args[1]);
            State* equals_1 = get_state(args[2], args[3]);
            State* equals_2 = get_state(args[4], args[5]);
            move_into->set_value<bool>(equals_1->equals(equals_2));
            equals_1->clean();
            equals_2->clean();
        } break;
        case toast::ADD:
        case toast::SUBTRACT:
        case toast::MULTIPLY:
        case toast::DIVIDE: {
            State* move_into = get_state(args[0], args[1]);
            State* op_1_state = get_state(args[2], args[3]);
            State* op_2_state = get_state(args[4], args[5]);
            int val = op_1_state->get_value<int>();
            int val_2 = op_2_state->get_value<int>();
            switch (type) {
                case toast::ADD: val += val_2; break;
                case toast::SUBTRACT: val -= val_2; break;
                case toast::MULTIPLY: val *= val_2; break;
                case toast::DIVIDE: val /= val_2; break;
            }
            move_into->set_value<int>(val);
        } break;
        case toast::IF: {
            State* state = get_state(args[0], args[1]);
            bool val = state->get_value<bool>();
            if (val) {
                position++;
            }
            state->clean();
        } break;
        case toast::LENGTH: {
            State* move_into = get_state(args[0], args[1]);
            State* state = get_state(args[2], args[3]);
            switch (state->get_type().get_main_type()) {
                case toast::STRING: {
                    std::string string = state->get_value<std::string>();
                    move_into->set_value<int>(string.size());
                } break;
                case toast::ARRAY: {
                    StateArray* array = state->get_value<StateArray*>();
                    move_into->set_value<int>(array->get_length());
                } break;
            }
            state->clean();
        } break;
        case toast::STREAM_IN: {
            State* stream_into = get_state(args[0], args[1]);
            State* stream_from = get_state(args[2], args[3]);
            switch (stream_into->get_type().get_main_type()) {
                case toast::STRING: {
                    std::stringstream stream;
                    std::string into = stream_into->get_value<std::string>();
                    std::string from = stream_from->get_value<std::string>();
                    stream << into;
                    stream << from;
                    stream_into->set_value<std::string>(stream.str());
                } break;
                case toast::ARRAY: {
                    StateArray* array = stream_into->get_value<StateArray*>();
                    // make a clone because if the state gets popped out of scope bad things will happen
                    State* state_clone = stream_from->clone();
                    array->push_state(state_clone);
                } break;
                default: {
                    StateArray* array = stream_from->get_value<StateArray*>();
                    stream_into->move_value_from(array->get_back());
                } break;
            }
            stream_from->clean();
        } break;
        case toast::STREAM_OUT: {
            State* stream_from = get_state(args[0], args[1]);
            bool has_stream_into = false;
            State* stream_into;
            if (args.size() > 2) {
                has_stream_into = true;
                stream_into = get_state(args[2], args[3]);
            }
            StateArray* array = stream_from->get_value<StateArray*>();
            State* state = array->pop_state();
            if (has_stream_into) {
                stream_into->move_value_from(state);
            }
            delete state;
        } break;
        case toast::DELETE: {
            State* state = get_state(args[0], args[1]);
            switch (state->get_type().get_main_type()) {
                case toast::ARRAY: {
                    StateArray* array = state->get_value<StateArray*>();
                    delete array;
                } break;
            }
        } break;
        default: {
            throw toast::Exception("No support for instruction");
        } break;
    }
}

t_vm::State::State(toast::StateTypeHolder type) {
    has_value = false;
    temp = false;
    this->type.push_back(type);
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
    if (stack->is_empty()) {
        pop_stack();
    }
    return state;
}

int t_vm::Frame::get_return() {
    return last;
}

toast::StateTypeHolder t_vm::State::get_type() {
    return type.back();
}

// std::vector<int> t_vm::State::get_value() {
//     return {};
// }



t_vm::State* t_vm::Stack::get_state(int offset) {
    State* state = states[(states.size() - 1) - offset];
    return state;
}

void t_vm::Stack::push_state(State* state) {
    states.push_back(state);
    // std::cout << "PUSH: " << states.size() << std::endl;
}

t_vm::State* t_vm::Stack::pop_state() {
    // std::cout << "POP: " << states.size() << std::endl;
    State* state = states.back();
    states.pop_back();
    return state;
}

t_vm::Frame::~Frame() {
    for (Stack* stack : stacks) {
        delete stack;
    }
}
t_vm::Stack::~Stack() {
    for (State* state : states) {
        delete state;
    }
}
t_vm::State::~State() {
    // delete type;
}


// void t_vm::State::set_value(std::vector<int> val) {
//     this->value = val;
// }

// void t_vm::State::set_value(int val) {
//     this->value.push_back(val);
// }

template<typename T>
void t_vm::State::set_value(T val) {
    has_value = true;
    value = val;
}

template<typename T>
T t_vm::State::get_value() {
    if (!has_value) {
        throw toast::Exception("Has no value");
    }
    return std::any_cast<T>(value);
}


bool t_vm::Stack::is_empty() {
    return states.size() == 0;
}

std::any t_vm::State::get_value_any() {
    return value;
}

void t_vm::State::move_value_from(State* state) {
    has_value = true;
    value = state->get_value_any();
}

bool t_vm::State::equals(State* state) {
    toast::StateTypeHolder type = get_type();
    toast::StateTypeHolder other_type = state->get_type();
    if (!type.equals(other_type)) {
        return false;
    }
    toast::StateType type_enum = type.get_main_type();
    switch (type_enum) {
        case toast::INT:
            return get_value<int>() == state->get_value<int>();
        case toast::BOOL:
            return get_value<bool>() == state->get_value<bool>();
        case toast::STRING:
            return get_value<std::string>() == state->get_value<std::string>();
        case toast::FUNC:
        case toast::ARRAY:
            return false;
        case toast::VOID:
            return true;
        default:
            throw toast::Exception("Unsupported type");
    }
}

void t_vm::State::set_temp(bool temp) {
    this->temp = temp;
}

void t_vm::State::clean() {
    if (temp) {
        delete this;
    }
}

t_vm::StateArray::StateArray() {
    
}

t_vm::StateArray::~StateArray() {
    
}

int t_vm::StateArray::get_length() {
    return states.size();
}

void t_vm::StateArray::push_state(State* state) {
    states.push_back(state);
}

t_vm::State* t_vm::StateArray::get_back() {
    return states.back();
}

t_vm::State* t_vm::StateArray::pop_state() {
    State* state = states.back();
    states.pop_back();
    return state;
}

t_vm::State* t_vm::State::clone() {
    State* clone = new State(get_type());
    clone->set_temp(temp);
    clone->move_value_from(this);
    return clone;
}

bool t_vm::State::is_empty() {
    return !has_value;
}