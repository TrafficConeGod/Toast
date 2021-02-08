#pragma once
#include "Token.h"
#include "Script.h"
#include <iostream>
#include <deque>

namespace toast {
    class Parser {
        private:
            Script* script;
        public:
            Parser(std::deque<Token> tokens);
            Script* get_script();
    };
}