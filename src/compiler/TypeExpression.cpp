#include "TypeExpression.h"
using namespace toast;
using namespace std;

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
    if (next_type == IDENT || next_type == RIGHT_ANGLE || next_type == LEFT_PAREN) {
        return;
    }
    switch (next.get_type()) {
        case LEFT_BRACKET: {
            tokens->pop_front();
            if (tokens->front().get_type() != RIGHT_BRACKET) {
                expected("]", tokens->front().get_literal());
            }
            tokens->pop_front();
            type_expressions.push_back(TypeExpression(type));
            type = StateType::ARRAY;
        } break;
        case LEFT_ANGLE: {
            tokens->pop_front();
            if (tokens->front().get_type() != RIGHT_ANGLE) {
                for (;;) {
                    Token token = tokens->front();
                    type_expressions.push_back(TypeExpression(tokens));
                    Token next = tokens->front();
                    tokens->pop_front();
                    if (next.get_type() == RIGHT_ANGLE) {
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
        case INT:
        case BOOL:
        case STRING:
        case FLOAT:
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