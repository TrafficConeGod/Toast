#pragma once
#include "Token.h"
#include "ScriptType.h"
#include "StatementType.h"
#include "Statement.h"
#include <iostream>
#include <vector>
#include <deque>

namespace toast {
    class Statement;
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