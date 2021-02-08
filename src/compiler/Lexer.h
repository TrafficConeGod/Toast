#pragma once
#include "Token.h"
#include "CompilerException.h"
#include <iostream>
#include <deque>
#include <sstream>

namespace toast {
    class Lexer {
        private:
            int position = 0;
            std::string source;
            std::deque<Token> tokens;
            void handle_char();
            std::string get_source_until_non_letter();
            std::string get_source_until_non_digit();
            std::string get_and_move_until_char(char until);
            bool is_ident(char ch);
        public:
            Lexer(std::string source);
            std::deque<Token> get_tokens();
    };
}