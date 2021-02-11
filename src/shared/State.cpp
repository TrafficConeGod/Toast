#include "../shared/State.h"
using namespace toast;

State::State() {
}

State::State(StateTypeHolder type) {
    has_value = false;
    temp = false;
    this->type.push_back(type);
}

State::State(StateTypeHolder type, std::any value) {
    has_value = true;
    temp = false;
    this->type.push_back(type);
    this->value = value;
}

State::~State() {
    // delete type;
    
}

StateTypeHolder State::get_type() {
    if (!has_value) {
        throw Exception("Has no type");
    }
    return type.back();
}

// void State::set_value(std::vector<int> val) {
//     this->value = val;
// }

// void State::set_value(int val) {
//     this->value.push_back(val);
// }

std::any State::get_value_any() {
    return value;
}

void State::move_value_from(State* state) {
    has_value = true;
    set_type(state->get_type());
    value = state->get_value_any();
}

void State::set_type(StateTypeHolder type) {
    if (this->type.size() == 0) {
        this->type.push_back(type);
    } else {
        this->type[0] = type;
    }
}

bool State::equals(State* state) {
    StateTypeHolder type = get_type();
    StateTypeHolder other_type = state->get_type();
    if (!type.equals(other_type)) {
        return false;
    }
    StateType type_enum = type.get_main_type();
    switch (type_enum) {
        case StateType::INT:
            return get_value<int>() == state->get_value<int>();
        case StateType::BOOL:
            return get_value<bool>() == state->get_value<bool>();
        case StateType::STRING:
            return get_value<std::string>() == state->get_value<std::string>();
        case StateType::FUNC:
        case StateType::ARRAY:
            return false;
        case StateType::VOID:
            return true;
        default:
            throw Exception("Unsupported type");
    }
}

void State::set_temp(bool temp) {
    this->temp = temp;
}

void State::clean() {
    if (temp) {
        delete this;
    }
}

State* State::clone() {
    State* clone = new State(get_type());
    clone->set_temp(temp);
    clone->move_value_from(this);
    return clone;
}

bool State::is_empty() {
    return !has_value;
}