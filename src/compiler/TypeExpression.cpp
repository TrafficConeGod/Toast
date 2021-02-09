#include "TypeExpression.h"
#include "TokenType.h"
#include "t_cmp.h"
using namespace toast;

TypeExpression::TypeExpression(toast::StateType type) {
    this->type = type;
}

TypeExpression::TypeExpression(std::deque<Token>* tokens) {
    Token token = tokens->front();
    std::string literal = token.get_literal();
    if (literal == "int") {
        type = StateType::INT;
    } else if (literal == "bool") {
        type = StateType::BOOL;
    } else if (literal == "string") {
        type = StateType::STRING;
    } else if (literal == "float") {
        type = StateType::FLOAT;
    } else if (literal == "function") {
        type = StateType::FUNC;
    } else if (literal == "array") {
        type = StateType::ARRAY;
    }

    StateTypeHolder cur_type = get_type();

    tokens->pop_front();
    Token next = tokens->front();
    TokenType next_type = next.get_type();
    if (next_type == TokenType::IDENT || next_type == TokenType::RIGHT_ANGLE || next_type == TokenType::LEFT_PAREN) {
        return;
    }
    switch (next.get_type()) {
        case TokenType::LEFT_BRACKET: {
            tokens->pop_front();
            if (tokens->front().get_type() != TokenType::RIGHT_BRACKET) {
                expected("]", tokens->front().get_literal());
            }
            tokens->pop_front();
            type_expressions.push_back(TypeExpression(type));
            type = StateType::ARRAY;
        } break;
        case TokenType::LEFT_ANGLE: {
            tokens->pop_front();
            if (tokens->front().get_type() != TokenType::RIGHT_ANGLE) {
                for (;;) {
                    Token token = tokens->front();
                    type_expressions.push_back(TypeExpression(tokens));
                    Token next = tokens->front();
                    tokens->pop_front();
                    if (next.get_type() == TokenType::RIGHT_ANGLE) {
                        break;
                    }
                }
            }
        } break;
        default: {
            expected("[ or <", next.get_literal());
        } break;
    }
}

StateType TypeExpression::get_type() {
    return type;
}

std::vector<TypeExpression> TypeExpression::get_type_expressions() {
    return type_expressions;
}

StateTypeHolder TypeExpression::get_type_holder() {
    switch (type) {
        case StateType::INT:
        case StateType::BOOL:
        case StateType::STRING:
        case StateType::FLOAT:
            return StateTypeHolder(type);
        default:
            std::vector<StateTypeHolder> sub_types = {};
            for (TypeExpression type_expr : type_expressions) {
                sub_types.push_back(type_expr.get_type_holder());
            }
            return StateTypeHolder(type, sub_types);
    }
}

void TypeExpression::add_expression(TypeExpression expr) {
    type_expressions.push_back(expr);
}