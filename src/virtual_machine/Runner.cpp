#include "Runner.h"
using namespace toast;
using namespace std;

Runner::Runner(vector<Instruction> instructions) {
    set_frame(0, true);
    this->instructions = instructions;
    for (position = 0; position < instructions.size(); position++) {
        handle_instruction();
    }
}

Runner::~Runner() {
    map<int, Frame*>::iterator it;
    for (it = frames.begin(); it != frames.end(); it++) {
        delete it->second;
    }
    for (State* state : call_args) {
        delete state;
    }
    // delete return_state;
}

void Runner::set_frame(int key, bool push_stack) {
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

State* Runner::get_state(int val, int offset) {
    if (val >= 0) {
        StateTypeHolder type((StateType) val);
        State* state = new State(type);
        state->set_temp(true);
        switch (state->get_type().get_main_type()) {
            case INT: {
                state->set_value<int>(offset);
            } break;
            case BOOL: {
                state->set_value<bool>(offset);
            } break;
            default: {
                throw Exception("No support for quick type");
            } break;
        }
        return state;
    } else {
        int key = (val + 1) * -1;
        if (frames.count(key) == 0) {
            throw Exception("Frame does not exist");
        }
        Frame* frame = frames[key];
        return frame->get_state(offset);
    }
}

State* Runner::push_state(StateTypeHolder type) {
    State* state = new State(type);
    Frame* frame = frames[frame_key];
    frame->push_state(state);
    return state;
}

State* Runner::pop_state() {
    Frame* frame = frames[frame_key];
    State* state = frame->pop_state();
    cout << "Popped state of type " << state->get_type().get_main_type();
    if (!state->is_empty()) {
        switch (state->get_type().get_main_type()) {
            case INT:
                cout << " and value " << state->get_value<int>();
                break;
            case BOOL:
                cout << " and value " << state->get_value<bool>();
                break;
            case STRING:
                cout << " and value " << state->get_value<string>();
                break;
        }
    }
    cout << endl;
    return state;
}

void Runner::handle_instruction() {
    Instruction instruction = instructions[position];
    InstructionType type = instruction.get_type();
    vector<int> args = instruction.get_args();
    cout << frame_key << " " << instruction.make_human_readable();
    switch (type) {
        case PUSH: {
            StateTypeHolder type(args);
            State* state = push_state(type);
        } break;
        case POP: {
            delete pop_state();
        } break;
        case SET: {
            State* state = get_state(args[0], args[1]);
            switch (state->get_type().get_main_type()) {
                case INT: {
                    state->set_value<int>(args[2]);
                } break;
                case BOOL: {
                    state->set_value<bool>(args[2]);
                } break;
                case FUNC: {
                    state->set_value<int>(position + 1);
                } break;
                case STRING: {
                    state->set_value<string>(instruction.get_string());
                } break;
                case ARRAY: {
                    StateArray* array = new StateArray();
                    state->set_value<StateArray*>(array);
                } break;
                default: {
                    throw Exception("No support for type");
                } break;
            }
            // state->clean();
        } break;
        case SKIP: {
            position += args[0];
        } break;
        case MOVE: {
            State* state = get_state(args[0], args[1]);
            State* from = get_state(args[2], args[3]);
            state->move_value_from(from);
            state->clean();
            from->clean();
        } break;
        case CALL: {
            State* state = get_state(args[0], args[1]);
            return_stack.push_back(position);
            position = state->get_value<int>();
            for (int i = 2; i < args.size(); i += 2) {
                State* arg_state = get_state(args[i], args[i + 1]);
                call_args.push_back(arg_state);
            }
            state->clean();
        } break;
        case EXIT: {
            position = return_stack.back();
            return_stack.pop_back();
        } break;
        case FRAME: {
            int frame_key = args[0];
            set_frame(frame_key, true);
        } break;
        case BACK: {
            Frame* frame = frames[frame_key];
            frames.erase(frame_key);
            set_frame(frame->get_return(), false);
            delete frame;
        } break;
        case ARG: {
            State* move_into = get_state(args[0], args[1]);
            State* arg_state = call_args.front();
            call_args.pop_front();
            move_into->move_value_from(arg_state);
            arg_state->clean();
        } break;
        case RETURN: {
            return_state = get_state(args[0], args[1]);
            // later add skipping code stuff
        } break;
        case MOVE_RETURN: {
            State* move_to = get_state(args[0], args[1]);
            move_to->move_value_from(return_state);
            return_state->clean();
            move_to->clean();
        } break;
        case EQUALS: {
            State* move_into = get_state(args[0], args[1]);
            State* equals_1 = get_state(args[2], args[3]);
            State* equals_2 = get_state(args[4], args[5]);
            move_into->set_value<bool>(equals_1->equals(equals_2));
            equals_1->clean();
            equals_2->clean();
        } break;
        case ADD:
        case SUBTRACT:
        case MULTIPLY:
        case DIVIDE: {
            State* move_into = get_state(args[0], args[1]);
            State* op_1_state = get_state(args[2], args[3]);
            State* op_2_state = get_state(args[4], args[5]);
            int val = op_1_state->get_value<int>();
            int val_2 = op_2_state->get_value<int>();
            switch (type) {
                case ADD: val += val_2; break;
                case SUBTRACT: val -= val_2; break;
                case MULTIPLY: val *= val_2; break;
                case DIVIDE: val /= val_2; break;
            }
            move_into->set_value<int>(val);
        } break;
        case IF: {
            State* state = get_state(args[0], args[1]);
            bool val = state->get_value<bool>();
            if (val) {
                position++;
            }
            state->clean();
        } break;
        case LENGTH: {
            State* move_into = get_state(args[0], args[1]);
            State* state = get_state(args[2], args[3]);
            switch (state->get_type().get_main_type()) {
                case STRING: {
                    string string = state->get_value<std::string>();
                    move_into->set_value<int>(string.size());
                } break;
                case ARRAY: {
                    StateArray* array = state->get_value<StateArray*>();
                    move_into->set_value<int>(array->get_length());
                } break;
            }
            state->clean();
        } break;
        case STREAM_IN: {
            State* stream_into = get_state(args[0], args[1]);
            State* stream_from = get_state(args[2], args[3]);
            switch (stream_into->get_type().get_main_type()) {
                case STRING: {
                    stringstream stream;
                    string into = stream_into->get_value<string>();
                    string from = stream_from->get_value<string>();
                    stream << into;
                    stream << from;
                    stream_into->set_value<string>(stream.str());
                } break;
                case ARRAY: {
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
        case STREAM_OUT: {
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
        case DELETE: {
            State* state = get_state(args[0], args[1]);
            switch (state->get_type().get_main_type()) {
                case ARRAY: {
                    StateArray* array = state->get_value<StateArray*>();
                    delete array;
                } break;
            }
        } break;
        default: {
            throw Exception("No support for instruction");
        } break;
    }
}