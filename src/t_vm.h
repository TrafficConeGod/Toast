#pragma once
#include <iostream>
#include <vector>
#include <map>
#include "toast.h"

namespace t_vm {

    class State {
        private:
            std::vector<toast::StateTypeHolder> type;
            std::vector<int> value;
        public:
            ~State();
            State(toast::StateTypeHolder type);
            toast::StateTypeHolder get_type();
            std::vector<int> get_value();
            void set_value(std::vector<int> val);
            void set_value(int val);
            std::string get_cpp_string();
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