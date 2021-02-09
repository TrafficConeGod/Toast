#pragma once
#include "../shared/StateType.h"
#include "../shared/StateTypeHolder.h"
#include "Token.h"
#include <iostream>
#include <vector>
#include <deque>

namespace toast {
    class TypeExpression {
        private:
            StateType type;
            std::vector<TypeExpression> type_expressions;
        public:
            TypeExpression(StateType type);
            TypeExpression(std::deque<Token>* tokens);
            StateType get_type();
            std::vector<TypeExpression> get_type_expressions();
            StateTypeHolder get_type_holder();
            void add_expression(TypeExpression expr);
    };
}