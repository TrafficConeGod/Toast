#pragma once
#include "../shared/StateTypeHolder.h"
#include "../shared/Exception.h"
#include <iostream>
#include <vector>
#include <any>

namespace toast {
    class State {
        private:
            std::vector<StateTypeHolder> type;
            bool has_value;
            std::any value;
            bool temp;
        public:
            State(StateTypeHolder type);
            ~State();
            StateTypeHolder get_type();
            template<typename T>
            T get_value();
            template<typename T>
            void set_value(T val);
            std::any get_value_any();
            void move_value_from(State* state);
            // void set_value(std::vector<int> val);
            // void set_value(int val);
            std::string get_cpp_string();
            bool equals(State* state);
            void set_temp(bool temp);
            void clean();
            State* clone();
            bool is_empty();
    };
}