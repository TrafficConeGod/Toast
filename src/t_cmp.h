#pragma once
#include <iostream>
#include <vector>
#include <map>
#include "toast.h"

namespace t_cmp {
    enum TokenType {
        ILLEGAL,
        FILE_END,
        IDENT,
        VAL,
        TRUE,
        FALSE,
        ASSIGN,
        PLUS,
        COMMA,
        LINE_END,
        LEFT_PAREN,
        RIGHT_PAREN,
        LEFT_BRACE,
        RIGHT_BRACE,
        TYPE,
        RETURN
    };

    class Token {
        private:
            TokenType type;
            std::string literal;
        public:
            Token(TokenType type, std::string literal);
            TokenType get_type();
            std::string get_literal();
    };

    class Lexer {
        private:
            int position = 0;
            std::string source;
            std::vector<Token*> tokens;
            void handle_char();
            std::string get_source_until_non_letter();
            std::string get_source_until_non_digit();
        public:
            Lexer(std::string source);
            std::vector<Token*> get_tokens();
    };

    class State {
        private:
            toast::StateTypeHolder* type;
            int stack_frame;
        public:
            State(toast::StateTypeHolder* type, int stack_frame);
            void set_stack_frame();
            toast::StateTypeHolder* get_type();
            int get_stack_frame();
    };

    enum ScopeType {
        GLOBAL,
        FUNCTION,
        BLOCK
    };

    class Scope {
        private:
            ScopeType type;
            std::vector<State*> state_stack;
            std::map<std::string, State*> var_map;
            int stack_frame;
        public:
            Scope(ScopeType type, int stack_frame);
            ~Scope();
            void push(State* state);
            std::vector<State*> get_state_stack();
            void add_var(std::string name, State* state);
            bool has_var(std::string name);
            int get_var_offset(std::string name);
            State* get_var(std::string name);
            ScopeType get_type();
            int get_stack_frame();
    };

    class Builder {
        private:
            int position = 0;
            std::vector<Token*> tokens;
            std::vector<toast::Instruction*> instructions;
            std::vector<Scope*> scope_stack;
            int stack_frame = 0;
            void handle_token();
            bool has_var(std::string name);
            int get_var_offset(std::string name);
            State* get_var(std::string name);
            void check_block(int position);
            void declare_var(std::string name, toast::StateTypeHolder* type);
            void set_var(std::string name, Token* token);
            void call_function(std::string name);
            int get_skip_amount(int position);
        public:
            Builder(std::vector<Token*> tokens);
            ~Builder();
            std::vector<toast::Instruction*> get_instructions();

    };

    std::vector<toast::Instruction*> generate_instruction_list(std::string source);
    std::string make_human_readable(std::vector<toast::Instruction*>);
    int parse_val(std::string literal, toast::StateTypeHolder* type);
    void delete_instruction_list(std::vector<toast::Instruction*> instructions);
}
