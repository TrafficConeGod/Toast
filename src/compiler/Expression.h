#pragma once
#include "../shared/Instruction.h"
#include "../shared/StateTypeHolder.h"
#include "ExpressionType.h"
#include "TypeExpression.h"
#include "Builder.h"
#include <iostream>
#include <vector>
#include <deque>
#include <any>

namespace toast {
    class Statement;
    class Expression {
        private:
            ExpressionType type;
            std::vector<Statement> statements;
            std::vector<Expression> expressions;
            std::vector<TypeExpression> type_expressions;
            std::vector<std::string> identifiers;
            std::vector<std::any> values;
            uint var_key;
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
            bool can_be_set();
            int get_value();
            std::string get_string_value();
            std::vector<Instruction> generate_push_instructions(Builder* builder);
            std::vector<Instruction> generate_pop_instructions(Builder* builder);
            std::vector<uint> get_args(Builder* builder);
            std::vector<State*> get_states(Builder* builder);
            StateTypeHolder get_type_holder(Builder* builder);
            void check_type(Builder* builder, Var* state);
            Var* get_var(Builder* builder);
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