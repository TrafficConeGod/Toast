#pragma once
#include "../shared/Instruction.h"
#include "ExpressionType.h"
#include "StatementType.h"
#include "Builder.h"
#include "Var.h"
#include "Token.h"
#include "TypeExpression.h"
#include <vector>
#include <deque>

namespace toast {
    class Expression;
    class Statement {
        private:
            StatementType type;
            std::vector<Statement> statements;
            std::vector<TypeExpression> type_expressions;
            std::vector<Expression> expressions;
            std::vector<std::string> identifiers;
            void handle_set(Builder* builder, std::vector<Instruction>* instructions, Var* state, Expression* expr);
        public:
            Statement(std::deque<Token>* tokens);
            StatementType get_type();
            std::vector<Statement> get_statements();
            std::vector<TypeExpression> get_type_expressions();
            std::vector<Expression> get_expressions();
            std::vector<std::string> get_identifiers();
            std::vector<Instruction> generate_instructions(Builder* builder);
            void clean();
    };
}

#include "Expression.h"