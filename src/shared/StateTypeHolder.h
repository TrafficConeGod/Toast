#pragma once
#include <iostream>
#include <vector>
#include "StateType.h"

namespace toast {
    class StateTypeHolder {
        private:
            StateType main_type;
            std::vector<StateTypeHolder> sub_types;
        public:
            StateTypeHolder(StateType main_type);
            StateTypeHolder(StateType main_type, std::vector<StateTypeHolder> sub_types);
            StateTypeHolder(std::vector<int> type_args);
            StateType get_main_type();
            std::vector<StateTypeHolder> get_sub_types();
            bool equals(StateTypeHolder type);
            bool equals(StateType type);
            std::vector<int> get_args();
    };
}