#include "State.h"
using namespace toast;
using namespace std;

State::State(StateTypeHolder type) {
    has_value = false;
    temp = false;
    this->type.push_back(type);
}

State::~State() {
    // delete type;
}

StateTypeHolder State::get_type() {
    return type.back();
}

// void State::set_value(vector<int> val) {
//     this->value = val;
// }

// void State::set_value(int val) {
//     this->value.push_back(val);
// }

any State::get_value_any() {
    return value;
}

void State::move_value_from(State* state) {
    has_value = true;
    value = state->get_value_any();
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
            return get_value<string>() == state->get_value<string>();
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