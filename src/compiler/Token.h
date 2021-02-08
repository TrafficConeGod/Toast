#pragma once
#include "TokenType.h"
#include <iostream>

namespace toast {
    class Token {
        private:
            TokenType type;
            std::string literal;
        public:
            Token(TokenType type, std::string literal);
            TokenType get_type();
            std::string get_literal();
            bool is_end();
    };
}