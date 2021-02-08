#pragma once
#include "ScriptType.h"
#include "Token.h"
#include "Statement.h"
#include <iostream>
#include <deque>

namespace toast {
    class Script {
        private:
            ScriptType type;
            std::vector<Statement> statements;
        public:
            Script(std::deque<Token>* tokens);
            std::vector<Statement> get_statements();
            void clean();
    };
}