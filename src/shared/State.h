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
            State();
            State(StateTypeHolder type);
            State(StateTypeHolder type, std::any value);
            ~State();
            StateTypeHolder get_type();
            void set_type(StateTypeHolder type);
            std::any get_value_any();
            void move_value_from(State* state);
            std::string get_cpp_string();
            bool equals(State* state);
            void set_temp(bool temp);
            void clean();
            State* clone();
            bool is_empty();
            // i love c++
            template<typename T>
            void set_value(T val) {
                has_value = true;
                value = val;
            }
            template<typename T>
            T get_value() {
                if (!has_value) {
                    throw Exception("Has no value");
                }
                return std::any_cast<T>(value);
            }
    };
}