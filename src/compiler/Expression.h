#pragma once
#include "../shared/StateType.h"
#include "ExpressionType.h"
#include "Statement.h"
#include "TypeExpression.h"
#include <iostream>
#include <vector>
#include <deque>

namespace toast {
    class Expression {
        private:
            ExpressionType type;
            std::vector<Statement> statements;
            std::vector<Expression> expressions;
            std::vector<TypeExpression> type_expressions;
            std::vector<std::string> identifiers;
            std::vector<std::any> values;
            void parse_middle(std::deque<Token>* tokens);
        public:
            Expression(ExpressionType type, std::vector<Statement> statements, std::vector<Expression> expressions, std::vector<TypeExpression> type_expressions, std::vector<std::string> identifiers, std::vector<std::any> values);
            Expression(std::deque<Token>* tokens);
            ExpressionType get_type();
            std::vector<Statement> get_statements();
            std::vector<Expression> get_expressions();
            std::vector<TypeExpression> get_type_expressions();
            std::vector<std::string> get_identifiers();
            std::vector<std::any> get_values();
            void clean();
            bool can_be_set() {
                switch (type) {
                    case INT:
                    case BOOL:
                    case STRING:
                        return true;
                    default:
                        return false;
                }
            }
            int get_value();
            std::string get_string_value();
            std::vector<toast::Instruction> generate_push_instructions(Builder* builder);
            std::vector<toast::Instruction> generate_pop_instructions(Builder* builder);
            std::vector<int> get_move_args(Builder* builder);
            toast::StateTypeHolder get_type_holder(Builder* builder);
            void check_type(Builder* builder, State* state);
            State* get_state(Builder* builder);
            // bool can_be_quick_moved() {
            //     switch (type) {
            //         case INT:
            //         case BOOL:
            //         case IDENTIFIER:
            //             return true;
            //         default:
            //             return false;
            //     }
            // }
    };
}