#include <iostream>
#include <vector>
#include <map>
#include "toast.h"

namespace t_cmp {
    enum ValueType {
        INT,
        BOOL
    };

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
        FUNC,
        TYPE,
        RET
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
        ENTER_SCOPE,
        EXIT_SCOPE,
        VAR_DECL,
        SET_VAR,
        PUSH_STATE,
        POP_STATE
    };

    class Instruction {
        private:
            InstructionType type;
            int* args;
        public:
            Instruction(InstructionType type, int* args);
            InstructionType get_type();
            int* get_args();
    };

    class Var {
        private:
            ValueType type;
            long index;
        public:
            Var(ValueType type, long index);
            ValueType get_type();
            long get_index();
    };

    class Builder {
        private:
            int position = 0;
            std::vector<Token*> tokens;
            std::vector<Instruction*> instructions;
            std::map<std::string, Var*> vars;
            long var_index = 0;
            void handle_token();
        public:
            Builder(std::vector<Token*> tokens);
            std::vector<Instruction*> get_instructions();

    };

    std::vector<Instruction*> generate_instruction_list(std::string source);
    std::string make_human_readable(std::vector<Instruction*>);
    int parse_val(std::string literal, ValueType type);
}