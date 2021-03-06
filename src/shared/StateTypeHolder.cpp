#include "StateTypeHolder.h"
using namespace toast;

StateTypeHolder::StateTypeHolder(StateType main_type) {
    this->main_type = main_type;
}

// void StateTypeHolder::func_init(StateTypeHolder return_type, std::vector<StateTypeHolder> func_args) {
//     this->return_type.push_back(return_type);
//     this->func_args = func_args;
// }

StateType StateTypeHolder::get_main_type() {
    return main_type;
}

// StateTypeHolder StateTypeHolder::get_return_type() {
//     return return_type.back();
// }

// std::vector<StateTypeHolder> StateTypeHolder::get_func_args() {
//     return func_args;
// }

bool StateTypeHolder::equals(StateTypeHolder type) {
    if (main_type != type.get_main_type()) {
        return false;
    }
    if (sub_types.size() != type.get_sub_types().size()) {
        return false;
    }
    for (int i = 0; i < sub_types.size(); i++) {
        StateTypeHolder type_1 = sub_types[i];
        StateTypeHolder type_2 = type.get_sub_types()[i];
        if (!type_1.equals(type_2)) {
            return false;
        }
    }
    return true;
}

bool StateTypeHolder::equals(StateType type) {
    return main_type == type;
}


StateTypeHolder::StateTypeHolder(StateType main_type, std::vector<StateTypeHolder> sub_types) {
    this->main_type = main_type;
    this->sub_types = sub_types;
}

StateTypeHolder::StateTypeHolder(std::vector<int> type_args) {
    main_type = (StateType) type_args[0];
    switch (main_type) {
        case StateType::FUNC:
        case StateType::ARRAY: {
            for (int i = 1; i < type_args.size(); i++) {
                int val = type_args[i];
                // inefficient code incoming
                // fill the std::vector with the section
                std::vector<int> sub_args;
                for (int j = i + 1; j < i + 1 + val; j++) {
                    sub_args.push_back(type_args[j]);
                }
                sub_types.push_back(StateTypeHolder(sub_args));
                i += val;
            }
        } break;
    }
}

std::vector<StateTypeHolder> StateTypeHolder::get_sub_types() {
    return sub_types;
}

std::vector<int> StateTypeHolder::get_args() {
    switch (main_type) {
        case StateType::INT:
        case StateType::BOOL:
        case StateType::STRING:
        case StateType::FLOAT:
            return { (int)main_type };
        default:
            std::vector<int> args = { (int)main_type };
            for (StateTypeHolder type : sub_types) {
                std::vector<int> sub_args = type.get_args();
                args.push_back(sub_args.size());
                for (int sub_arg : sub_args) {
                    args.push_back(sub_arg);
                }
            }
            return args;
    }
}