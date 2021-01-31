#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <deque>
#include "toast.h"
#include <any>

namespace t_vm {

    class State {
        private:
            std::vector<toast::StateTypeHolder> type;
            bool has_value;
            std::any value;
            bool temp;
        public:
            ~State();
            State(toast::StateTypeHolder type);
            toast::StateTypeHolder get_type();
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

    class StateArray {
        private:
            std::vector<State*> states_holding;
            std::vector<State*> states;
        public:
            ~StateArray();
            StateArray();
            int get_length();
            void push_state(State* state);
            State* get_back();
            State* pop_state();
            void set_state(int index, State* state);
            State* get_state(int index);
    };

    class Stack {
        private:
            std::vector<State*> states;
        public:
            ~Stack();
            void push_state(State* state);
            State* pop_state();
            State* get_state(int offset);
            bool is_empty();
    };

    class Frame {
        private:
            int last;
            std::vector<int> keys;
            std::vector<Stack*> stacks;
        public:
            ~Frame();
            void push_stack(int key);
            void pop_stack();
            void push_state(State* state);
            State* pop_state();
            State* get_state(int offset);
            int get_return();
    };

    class Runner {
        private:
            int position = 0;
            int frame_key = -1;
            std::vector<toast::Instruction> instructions;
            std::map<int, Frame*> frames;
            std::vector<int> return_stack;
            std::deque<State*> call_args;
            State* return_state;
        public:
            Runner(std::vector<toast::Instruction>);
            ~Runner();
            void handle_instruction();
            void set_frame(int frame_key, bool push_stack);
            State* get_state(int frame_key, int offset);
            State* push_state(toast::StateTypeHolder type);
            State* pop_state();
    };

    void execute(std::vector<toast::Instruction> instructions);
}