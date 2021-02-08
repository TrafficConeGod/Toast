#include "Expression.h"
using namespace toast;
using namespace std;

Expression::Expression(ExpressionType type, std::vector<Statement> statements, std::vector<Expression> expressions, std::vector<TypeExpression> type_expressions, std::vector<std::string> identifiers, std::vector<std::any> values) {
    this->type = type;
    this->statements = statements;
    this->expressions = expressions;
    this->type_expressions = type_expressions;
    this->identifiers = identifiers;
    this->values = values;
}

Expression::Expression(std::deque<Token>* tokens) {
    Token token = tokens->front();
    switch (token.get_type()) {
        // 10 20 34 193 etc
        case INT_LITERAL: {
            type = ExpressionType::INT;
            values.push_back(parse_val(token.get_literal(), INT));
            tokens->pop_front();
        } break;
        // true false
        case BOOL_LITERAL: {
            type = ExpressionType::BOOL;
            values.push_back(parse_val(token.get_literal(), BOOL));
            tokens->pop_front();
        } break;
        // "Hello world!"
        case STR_LITERAL: {
            type = ExpressionType::STRING;
            values.push_back(token.get_literal());
            tokens->pop_front();
        } break;
        // var_name
        case IDENT: {
            type = IDENTIFIER;
            identifiers.push_back(token.get_literal());
            tokens->pop_front();
        } break;
        // (expression)
        case LEFT_PAREN: {
            tokens->pop_front();
            Expression expression = Expression(tokens);
            type = expression.get_type();
            statements = expression.get_statements();
            expressions = expression.get_expressions();
            type_expressions = expression.get_type_expressions();
            identifiers = expression.get_identifiers();
            values = expression.get_values();
            Token right_paren = tokens->front();
            if (right_paren.get_type() != RIGHT_PAREN) {
                expected(")", right_paren.get_literal());
            }
            tokens->pop_front();
        } break;
        // [expression, expression]
        case LEFT_BRACKET: {
            type = ExpressionType::ARRAY;
            tokens->pop_front();
            if (tokens->front().get_type() != RIGHT_BRACKET) {
                for (;;) {
                    Token token = tokens->front();
                    expressions.push_back(Expression(tokens));
                    Token next = tokens->front();
                    tokens->pop_front();
                    if (next.get_type() == RIGHT_BRACKET) {
                        break;
                    }
                }
            }
        } break;
        // !expression
        case EXCLAMATION: {
            type = NOT;
            tokens->pop_front();
            expressions.push_back(Expression(tokens));
        } break;
        // #expression
        case HASH: {
            type = ExpressionType::LENGTH;
            tokens->pop_front();
            expressions.push_back(Expression(tokens));
        } break;
        // type(type identifier) {}
        case TYPE_IDENT: {
            type = FUNCTION;
            type_expressions.push_back(TypeExpression(tokens));
            Token left_paren = tokens->front();
            if (left_paren.get_type() != LEFT_PAREN) {
                expected("(", left_paren.get_literal());
            }
            tokens->pop_front();
            if (tokens->front().get_type() != RIGHT_PAREN) {
                for (;;) {
                    type_expressions.push_back(TypeExpression(tokens));
                    Token ident = tokens->front();
                    if (ident.get_type() != IDENT) {
                        expected("identifier", ident.get_literal());
                        throw CompilerException();
                    }
                    identifiers.push_back(ident.get_literal());
                    tokens->pop_front();
                    if (tokens->front().get_type() == RIGHT_PAREN) {
                        break;
                    }
                    tokens->pop_front();
                }
            }
            tokens->pop_front();
            statements.push_back(Statement(tokens));
        } break;
        case FILE_END:
        case NEW_LINE: {
            type = EX_IGNORE;
            tokens->pop_front();
        } break;
        default: {
            expected("expression", token.get_literal());
        } break;
    }
    parse_middle(tokens);
}

void Expression::parse_middle(std::deque<Token>* tokens) {
    Token middle = tokens->front();
    TokenType middle_type = middle.get_type();
    if (middle.is_end()) {
        return;
    }
    switch (middle.get_type()) {
        case LEFT_PAREN:
        case PLUS:
        case MINUS:
        case TIMES:
        case OVER:
        case DOUBLE_EQUALS:
        case EXCLAMATION_EQUALS:
        case DOUBLE_AMPERSAND:
        case DOUBLE_VERT_BAR:
        case LEFT_BRACKET:
            break;
        default:
            return;
    }
    tokens->pop_front();
    ExpressionType old_type = type;
    switch (middle.get_type()) {
        case LEFT_PAREN: {
            type = FUNCTION_CALL;
            if (tokens->front().get_type() != RIGHT_PAREN) {
                for (;;) {
                    Token token = tokens->front();
                    expressions.push_back(Expression(tokens));
                    Token next = tokens->front();
                    tokens->pop_front();
                    if (next.get_type() == RIGHT_PAREN) {
                        break;
                    }
                }
            }
            parse_middle(tokens);
            return;
        }
        case PLUS:
            type = ExpressionType::ADD;
            break;
        case MINUS:
            type = ExpressionType::SUBTRACT;
            break;
        case TIMES:
            type = ExpressionType::MULTIPLY;
            break;
        case OVER:
            type = ExpressionType::DIVIDE;
            break;
        case DOUBLE_EQUALS:
            type = IS;
            break;
        case EXCLAMATION_EQUALS:
            type = NOT_IS;
            break;
        case DOUBLE_AMPERSAND:
            type = AND;
            break;
        case DOUBLE_VERT_BAR:
            type = OR;
            break;
        case LEFT_BRACKET:
            type = ARRAY_INDEX;
            expressions.push_back(Expression(tokens));
            if (tokens->front().get_type() != RIGHT_BRACKET) {
                expected("]", tokens->front().get_literal());
            }
            tokens->pop_front();
            return;
        default:
            return;
    }
    Expression expression = Expression(old_type, statements, expressions, type_expressions, identifiers, values);
    statements = {};
    expressions = { expression };
    type_expressions = {};
    expressions.push_back(Expression(tokens));
}

ExpressionType Expression::get_type() {
    return type;
}

std::vector<Statement> Expression::get_statements() {
    return statements;
}

std::vector<Expression> Expression::get_expressions() {
    return expressions;
}

std::vector<TypeExpression> Expression::get_type_expressions() {
    return type_expressions;
}

std::vector<std::string> Expression::get_identifiers() {
    return identifiers;
}

std::vector<std::any> Expression::get_values() {
    return values;
}

void Expression::clean() {
    {
        std::vector<Statement> cleaned;
        for (Statement statement : statements) {
            if (statement.get_type() != IGNORE) {
                statement.clean();
                cleaned.push_back(statement);
            }
        }
        statements = cleaned;
    }
    {
        std::vector<Expression> cleaned;
        for (Expression expression : expressions) {
            if (expression.get_type() != EX_IGNORE) {
                expression.clean();
                cleaned.push_back(expression);
            }
        }
        expressions = cleaned;
    }
}

int Expression::get_value() {
    return std::any_cast<int>(values.back());
}

std::string Expression::get_string_value() {
    return std::any_cast<std::string>(values.back());
}

std::vector<Instruction> Expression::generate_push_instructions(Builder* builder) {
    std::vector<Instruction> instructions;
    switch (type) {
        case ADD:
        case SUBTRACT:
        case MULTIPLY:
        case DIVIDE: {
            Expression expr_1 = expressions[0];
            Expression expr_2 = expressions[1];
            merge(&instructions, expr_1.generate_push_instructions(builder));
            merge(&instructions, expr_2.generate_push_instructions(builder));
            builder->add_temp_offset();
            instructions.push_back(Instruction(PUSH, { INT }));
            InstructionType instruction_type;
            switch (type) {
                case ADD:
                    instruction_type = ADD;
                    break;
                case SUBTRACT:
                    instruction_type = SUBTRACT;
                    break;
                case MULTIPLY:
                    instruction_type = MULTIPLY;
                    break;
                case DIVIDE:
                    instruction_type = DIVIDE;
                    break;
            }
            std::vector<int> args = get_move_args(builder);
            std::vector<int> args_1 = expr_1.get_move_args(builder);
            std::vector<int> args_2 = expr_2.get_move_args(builder);
            args_1 = offset_args(args_1, builder->get_temp_offset() - 1);
            args_2 = offset_args(args_2, builder->get_temp_offset() - 1);
            merge(&args, args_1);
            merge(&args, args_2);
            instructions.push_back(Instruction(instruction_type, args));
        } break;
        case FUNCTION: {
            builder->add_temp_offset();
            instructions.push_back(Instruction(PUSH, get_type_holder(builder).get_args()));
            instructions.push_back(Instruction(InstructionType::SET, get_move_args(builder)));
            Scope* scope = new Scope(FUNC, builder->get_frame() + 1);
            builder->push_scope(scope);
            Statement sub_statement = statements.back();
            std::vector<Instruction> sub_instructions = sub_statement.generate_instructions(builder);
            instructions.push_back(Instruction(SKIP, { (int)sub_instructions.size() + 2 }));
            merge(&instructions, sub_instructions);
            instructions.push_back(Instruction(BACK, {}));
            instructions.push_back(Instruction(EXIT, {}));
            delete builder->pop_scope();
        } break;
    }
    return instructions;
}

std::vector<Instruction> Expression::generate_pop_instructions(Builder* builder) {
    std::vector<Instruction> instructions;
    switch (type) {
        case ADD:
        case SUBTRACT:
        case MULTIPLY:
        case DIVIDE: {
            Expression expr_1 = expressions[0];
            Expression expr_2 = expressions[1];
            merge(&instructions, expr_1.generate_pop_instructions(builder));
            merge(&instructions, expr_2.generate_pop_instructions(builder));
            builder->sub_temp_offset();
            instructions.push_back(Instruction(POP, {}));
        } break;
        case FUNCTION:
            builder->sub_temp_offset();
            instructions.push_back(Instruction(POP, {}));
            break;
    }
    return instructions;
}

std::vector<int> Expression::get_move_args(Builder* builder) {
    switch (type) {
        case ExpressionType::INT:
            return { StateType::INT, get_value() };
        case ExpressionType::BOOL:
            return { StateType::BOOL, get_value() };
        case ExpressionType::IDENTIFIER: {
            std::string ident = identifiers.back();
            State* state = builder->get_state(ident);
            return { frame_negate(state->get_frame()), state->get_offset() };
        } break;
        default:
            return { frame_negate(builder->get_frame()), 0 };
    }
}

StateTypeHolder Expression::get_type_holder(Builder* builder) {
    switch (type) {
        case ExpressionType::INT:
            return StateTypeHolder(INT);
        case ExpressionType::BOOL:
            return StateTypeHolder(BOOL);
        case ExpressionType::STRING:
            return StateTypeHolder(STRING);
        case ExpressionType::IDENTIFIER: {
            std::string ident = identifiers.back();
            if (!builder->has_state(ident)) {
                not_declared(ident);
            }
            State* state = builder->get_state(ident);
            return state->get_type();
        }
        case ExpressionType::FUNCTION: {
            std::vector<StateTypeHolder> sub_types = {};
            for (TypeExpression type_expr : type_expressions) {
                sub_types.push_back(type_expr.get_type_holder());
            }
            return StateTypeHolder(StateType::FUNC, sub_types);
        }
        default: {
            Expression expr = expressions.back();
            return expr.get_type_holder(builder);
        }
    }
}

void Expression::check_type(Builder* builder, State* state) {
    if (!state->get_type().equals(get_type_holder(builder))) {
        std::cout << "Types are incompatible" << std::endl;
        throw CompilerException();
    }
}

State* Expression::get_state(Builder* builder) {
    switch (type) {
        case IDENTIFIER: {
            std::string ident = identifiers.back();
            if (!builder->has_state(ident)) {
                not_declared(ident);
            }
            State* state = builder->get_state(ident);
            return state;
        } break;
        default:
            std::cout << "Not identifiying" << std::endl;
            throw CompilerException();
    }
}