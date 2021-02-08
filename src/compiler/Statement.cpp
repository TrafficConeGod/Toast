#include "Statement.h"
using namespace toast;
using namespace std;

Statement::Statement(std::deque<Token>* tokens) {
    Token token = tokens->front();
    switch (token.get_type()) {
        // Compound
        case LEFT_BRACE: {
            type = COMPOUND;
            tokens->pop_front();
            do {
                Token token = tokens->front();
                statements.push_back(Statement(tokens));
            } while (tokens->front().get_type() != RIGHT_BRACE);
            tokens->pop_front();
        } break;
        // If statement
        case IF_WORD: {
            type = IF;
            tokens->pop_front();
            expressions.push_back(Expression(tokens));
            statements.push_back(Statement(tokens));
        } break;
        // else if statement
        case ELSE_WORD: {
            tokens->pop_front();
            Token next = tokens->front();
            if (next.get_type() == IF_WORD) {
                type = ELSE_IF;
                tokens->pop_front();
                expressions.push_back(Expression(tokens));
            } else {
                type = ELSE;
            }
            statements.push_back(Statement(tokens));
        } break;
        // Return statement
        case RETURN_WORD: {
            type = RETURN;
            tokens->pop_front();
            expressions.push_back(Expression(tokens));
            Token end = tokens->front();
            if (!end.is_end()) {
                expected("end", end.get_literal());
            }
        } break;
        // Delete statement
        case DELETE_WORD: {
            type = DELETE;
            tokens->pop_front();
            expressions.push_back(Expression(tokens));
            Token end = tokens->front();
            if (!end.is_end()) {
                expected("end", end.get_literal());
            }
        } break;
        // type ident = expression
        case TYPE_IDENT: {
            type_expressions.push_back(TypeExpression(tokens));
            Token ident = tokens->front();
            if (ident.get_type() != IDENT) {
                expected("identifier", ident.get_literal());
            }
            identifiers.push_back(ident.get_literal());
            tokens->pop_front();
            Token middle = tokens->front();
            if (middle.is_end()) {
                type = VAR_DECLARE;
                break;
            }
            if (middle.get_type() == LEFT_PAREN) {
                type = FUNCTION_DECLARE;
                TypeExpression return_expr = type_expressions.back();
                TypeExpression func_expr = TypeExpression(StateType::FUNC);
                func_expr.add_expression(return_expr);
                type_expressions[0] = func_expr;
                tokens->pop_front();
                if (tokens->front().get_type() != RIGHT_PAREN) {
                    for (;;) {
                        func_expr.add_expression(TypeExpression(tokens));
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
                Token end = tokens->front();
                if (!end.is_end()) {
                    type = FUNCTION_CREATE;
                    statements.push_back(Statement(tokens));
                }
                break;
            }
            if (middle.get_type() != EQUALS) {
                expected("= or (", middle.get_literal());
            }
            type = VAR_CREATE;
            tokens->pop_front();
            expressions.push_back(Expression(tokens));
            Token end = tokens->front();
            if (!end.is_end()) {
                expected("end", end.get_literal());
            }
        } break;
        case FILE_END:
        case NEW_LINE: {
            type = IGNORE;
            tokens->pop_front();
        } break;
        // expression = expression
        default: {
            expressions.push_back(tokens);
            Token middle = tokens->front();
            switch (middle.get_type()) {
                case EQUALS:
                    type = SET;
                    break;
                case PLUS_EQUALS:
                    type = ADD_SET;
                    break;
                case MINUS_EQUALS:
                    type = SUBTRACT_SET;
                    break;
                case TIMES_EQUALS:
                    type = MULTIPLY_SET;
                    break;
                case OVER_EQUALS:
                    type = DIVIDE_SET;
                    break;
                case DOUBLE_PLUS:
                    type = INCREMENT;
                    break;
                case DOUBLE_MINUS:
                    type = DECREMENT;
                    break;
                case LEFT_ANGLE: {
                    tokens->pop_front();
                    Token next = tokens->front();
                    if (next.get_type() != LEFT_ANGLE) {
                        expected("<", next.get_literal());
                    }
                    type = STREAM_INTO;
                } break;
                case RIGHT_ANGLE: {
                    tokens->pop_front();
                    Token next = tokens->front();
                    if (next.get_type() != RIGHT_ANGLE) {
                        expected(">", next.get_literal());
                    }
                    type = STREAM_OUT;
                } break;
                case RIGHT_PAREN:
                    type = EMPTY;
                    break;
                default:
                    // // try to make an expression
                    // tokens->push_front(token);
                    // expressions.push_back(Expression(tokens));
                    // return;
                    expected("= += -= *= /= ++ -- << or >>", middle.get_literal());
            }
            tokens->pop_front();
            if (!tokens->front().is_end()) {
                expressions.push_back(Expression(tokens));
            }
        } break;
    }
}

std::vector<Statement> Statement::get_statements() {
    return statements;
}
std::vector<Expression> Statement::get_expressions() {
    return expressions;
}
std::vector<TypeExpression> Statement::get_type_expressions() {
    return type_expressions;
}
StatementType Statement::get_type() {
    return type;
}

void Statement::clean() {
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

void Statement::handle_set(Builder* builder, std::vector<Instruction>* instructions, State* state, Expression expr) {
    expr.check_type(builder, state);
    if (expr.can_be_set()) {
        std::vector<int> args = state->get_args();
        if (state->get_type().equals(StateType::STRING)) {
            instructions->push_back(Instruction(InstructionType::SET, args, expr.get_string_value()));
        } else {
            args.push_back(expr.get_value());
            instructions->push_back(Instruction(InstructionType::SET, args));
        }
    } else {
        merge(instructions, expr.generate_push_instructions(builder));
        builder->update_state(state);
        std::vector<int> args = state->get_args();
        merge(&args, expr.get_move_args(builder));
        instructions->push_back(Instruction(InstructionType::MOVE, args));
        merge(instructions, expr.generate_pop_instructions(builder));
    }
}

std::vector<Instruction> Statement::generate_instructions(Builder* builder) {
    std::vector<Instruction> instructions;
    switch (type) {
        case VAR_CREATE:
        case VAR_DECLARE:
        case FUNCTION_CREATE:
        case FUNCTION_DECLARE: {
            TypeExpression type_expr = type_expressions.back();
            instructions.push_back(Instruction(InstructionType::PUSH, type_expr.get_type_holder().get_args()));
            std::string ident = identifiers.back();
            if (builder->has_state(ident)) {
                already_declared(ident);
            }
            builder->push_state(ident, new State(type_expr.get_type_holder(), builder->get_frame()));
            State* state = builder->get_state(ident);
            if (type == VAR_CREATE) {
                Expression expr = expressions.back();
                handle_set(builder, &instructions, state, expr);
            } else if (type == FUNCTION_CREATE) {
                instructions.push_back(Instruction(InstructionType::SET, { frame_negate(builder->get_frame()), 0 }));
                Scope* scope = new Scope(FUNC, builder->get_frame() + 1);
                builder->push_scope(scope);
                Statement sub_statement = statements.back();
                std::vector<Instruction> sub_instructions = sub_statement.generate_instructions(builder);
                instructions.push_back(Instruction(InstructionType::SKIP, { (int)sub_instructions.size() + 2 }));
                merge(&instructions, sub_instructions);
                instructions.push_back(Instruction(InstructionType::BACK, {}));
                instructions.push_back(Instruction(InstructionType::EXIT, {}));
                delete builder->pop_scope();
            }
        } break;
        case COMPOUND: {
            for (Statement sub_statement : statements) {
                merge(&instructions, sub_statement.generate_instructions(builder));
            }
        } break;
        case ADD_SET:
        case SUBTRACT_SET:
        case MULTIPLY_SET:
        case DIVIDE_SET:
        case SET: {
            Expression expr = expressions.front();
            Expression expr_from = expressions[1];
            merge(&instructions, expr.generate_push_instructions(builder));
            State* state = expr.get_state(builder);
            if (type == SET) {
                handle_set(builder, &instructions, state, expr_from);
            } else {
                std::vector<int> args = state->get_args();
                merge(&args, args);
                merge(&args, expr_from.get_move_args(builder));
                instructions.push_back(Instruction(InstructionType::ADD, args));
            }
            merge(&instructions, expr.generate_pop_instructions(builder));
        } break;
    }
    return instructions;
}
