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

    enum InstructionType {
        NONE,
        PUSH,
        POP,
        SET,
        MOVE,
        CALL,
        EXIT_FUNC
    };

    class Instruction {
        private:
            InstructionType type;
            std::vector<int> args;
        public:
            Instruction(InstructionType type, std::vector<int> args);
            InstructionType get_type();
            std::vector<int> get_args();
    };

    class State {
        private:
            toast::StateTypeHolder* type;
        public:
            State(toast::StateTypeHolder* type);
            toast::StateTypeHolder* get_type();
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
        public:
            Scope(ScopeType type);
            ~Scope();
            void push(State* state);
            std::vector<State*> get_state_stack();
            void add_var(std::string name, State* state);
            bool has_var(std::string name);
            int get_var_offset(std::string name);
            State* get_var(std::string name);
            ScopeType get_type();
    };

    class Builder {
        private:
            int position = 0;
            std::vector<Token*> tokens;
            std::vector<Instruction*> instructions;
            std::vector<Scope*> scope_stack;
            void handle_token();
            bool has_var(std::string name);
            int get_var_offset(std::string name);
            State* get_var(std::string name);
            void check_block(int position);
        public:
            Builder(std::vector<Token*> tokens);
            ~Builder();
            std::vector<Instruction*> get_instructions();

    };

    std::vector<Instruction*> generate_instruction_list(std::string source);
    std::string make_human_readable(std::vector<Instruction*>);
    int parse_val(std::string literal, toast::StateTypeHolder* type);
    void delete_instruction_list(std::vector<Instruction*> instructions);
}
