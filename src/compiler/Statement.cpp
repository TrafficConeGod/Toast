#include "Statement.h"
#include "../shared/Instruction.h"
#include "StatementType.h"
#include "Token.h"
#include "TokenType.h"
#include "Var.h"
#include "Scope.h"
#include "ExpressionType.h"
#include "Expression.h"
#include "CompilerException.h"
#include "Var.h"
#include "t_cmp.h"
using namespace toast;

Statement::Statement(std::deque<Token>* tokens) {
    Token token = tokens->front();
    switch (token.get_type()) {
        // Compound
        case TokenType::LEFT_BRACE: {
            type = StatementType::COMPOUND;
            tokens->pop_front();
            do {
                Token token = tokens->front();
                statements.push_back(Statement(tokens));
            } while (tokens->front().get_type() != TokenType::RIGHT_BRACE);
            tokens->pop_front();
        } break;
        // If statement
        case TokenType::IF_WORD: {
            type = StatementType::IF;
            tokens->pop_front();
            expressions.push_back(Expression(tokens));
            statements.push_back(Statement(tokens));
        } break;
        // else if statement
        case TokenType::ELSE_WORD: {
            tokens->pop_front();
            Token next = tokens->front();
            if (next.get_type() == TokenType::IF_WORD) {
                type = StatementType::ELSE_IF;
                tokens->pop_front();
                expressions.push_back(Expression(tokens));
            } else {
                type = StatementType::ELSE;
            }
            statements.push_back(Statement(tokens));
        } break;
        // Return statement
        case TokenType::RETURN_WORD: {
            type = StatementType::RETURN;
            tokens->pop_front();
            expressions.push_back(Expression(tokens));
            Token end = tokens->front();
            if (!end.is_end()) {
                expected("end", end.get_literal());
            }
        } break;
        // Delete statement
        case TokenType::DELETE_WORD: {
            type = StatementType::DELETE;
            tokens->pop_front();
            expressions.push_back(Expression(tokens));
            Token end = tokens->front();
            if (!end.is_end()) {
                expected("end", end.get_literal());
            }
        } break;
        // type ident = expression
        case TokenType::TYPE_IDENT: {
            type_expressions.push_back(TypeExpression(tokens));
            Token ident = tokens->front();
            if (ident.get_type() != TokenType::IDENT) {
                expected("identifier", ident.get_literal());
            }
            identifiers.push_back(ident.get_literal());
            tokens->pop_front();
            Token middle = tokens->front();
            if (middle.is_end()) {
                type = StatementType::VAR_DECLARE;
                break;
            }
            if (middle.get_type() == TokenType::LEFT_PAREN) {
                type = StatementType::FUNCTION_DECLARE;
                TypeExpression return_expr = type_expressions.back();
                TypeExpression func_expr = TypeExpression(StateType::FUNC);
                func_expr.add_expression(return_expr);
                type_expressions[0] = func_expr;
                tokens->pop_front();
                if (tokens->front().get_type() != TokenType::RIGHT_PAREN) {
                    for (;;) {
                        func_expr.add_expression(TypeExpression(tokens));
                        Token ident = tokens->front();
                        if (ident.get_type() != TokenType::IDENT) {
                            expected("identifier", ident.get_literal());
                            throw CompilerException();
                        }
                        identifiers.push_back(ident.get_literal());
                        tokens->pop_front();
                        if (tokens->front().get_type() == TokenType::RIGHT_PAREN) {
                            break;
                        }
                        tokens->pop_front();
                    }
                }
                tokens->pop_front();
                Token end = tokens->front();
                if (!end.is_end()) {
                    type = StatementType::FUNCTION_CREATE;
                    statements.push_back(Statement(tokens));
                }
                break;
            }
            if (middle.get_type() != TokenType::EQUALS) {
                expected("= or (", middle.get_literal());
            }
            type = StatementType::VAR_CREATE;
            tokens->pop_front();
            expressions.push_back(Expression(tokens));
            Token end = tokens->front();
            if (!end.is_end()) {
                expected("end", end.get_literal());
            }
        } break;
        case TokenType::FILE_END:
        case TokenType::NEW_LINE: {
            type = StatementType::IGNORE;
            tokens->pop_front();
        } break;
        // expression = expression
        default: {
            expressions.push_back(tokens);
            Token middle = tokens->front();
            switch (middle.get_type()) {
                case TokenType::EQUALS:
                    type = StatementType::SET;
                    break;
                case TokenType::PLUS_EQUALS:
                    type = StatementType::ADD_SET;
                    break;
                case TokenType::MINUS_EQUALS:
                    type = StatementType::SUBTRACT_SET;
                    break;
                case TokenType::TIMES_EQUALS:
                    type = StatementType::MULTIPLY_SET;
                    break;
                case TokenType::OVER_EQUALS:
                    type = StatementType::DIVIDE_SET;
                    break;
                case TokenType::DOUBLE_PLUS:
                    type = StatementType::INCREMENT;
                    break;
                case TokenType::DOUBLE_MINUS:
                    type = StatementType::DECREMENT;
                    break;
                case TokenType::LEFT_ANGLE: {
                    tokens->pop_front();
                    Token next = tokens->front();
                    if (next.get_type() != TokenType::LEFT_ANGLE) {
                        expected("<", next.get_literal());
                    }
                    type = StatementType::STREAM_INTO;
                } break;
                case TokenType::RIGHT_ANGLE: {
                    tokens->pop_front();
                    Token next = tokens->front();
                    if (next.get_type() != TokenType::RIGHT_ANGLE) {
                        expected(">", next.get_literal());
                    }
                    type = StatementType::STREAM_OUT;
                } break;
                case TokenType::RIGHT_PAREN:
                    type = StatementType::EMPTY;
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
            if (statement.get_type() != StatementType::IGNORE) {
                statement.clean();
                cleaned.push_back(statement);
            }
        }
        statements = cleaned;
    }
    {
        std::vector<Expression> cleaned;
        for (Expression expression : expressions) {
            if (expression.get_type() != ExpressionType::IGNORE) {
                expression.clean();
                cleaned.push_back(expression);
            }
        }
        expressions = cleaned;
    }
}

void Statement::handle_set(Builder* builder, std::vector<Instruction>* instructions, Var* var, Expression expr) {
    expr.check_type(builder, var);
    std::vector<uint> args = var->get_args();
    merge(instructions, expr.generate_push_instructions(builder));
    merge(&args, expr.get_args(builder));
    instructions->push_back(Instruction(InstructionType::MOVE, args, expr.get_states(builder)));
    merge(instructions, expr.generate_pop_instructions(builder));
    // if (expr.can_be_set()) {
    //     std::vector<uint> args = var->get_args();
    //     if (var->get_type().equals(StateType::STRING)) {
    //         // instructions->push_back(Instruction(InstructionType::SET, args, expr.get_string_value()));
    //     } else {
    //         args.push_back(expr.get_value());
    //         // instructions->push_back(Instruction(InstructionType::SET, args));
    //     }
    // } else {
    //     merge(instructions, expr.generate_push_instructions(builder));
    //     std::vector<uint> args = var->get_args();
    //     merge(&args, expr.get_args(builder));
    //     // instructions->push_back(Instruction(InstructionType::MOVE, args));
    //     merge(instructions, expr.generate_pop_instructions(builder));
    // }
}

std::vector<Instruction> Statement::generate_instructions(Builder* builder) {
    std::vector<Instruction> instructions;
    switch (type) {
        case StatementType::VAR_CREATE:
        case StatementType::VAR_DECLARE:
        case StatementType::FUNCTION_CREATE:
        case StatementType::FUNCTION_DECLARE: {
            TypeExpression type_expr = type_expressions.back();
            std::string ident = identifiers.back();
            if (builder->has_var(ident)) {
                already_declared(ident);
            }
            Var* var = new Var(type_expr.get_type_holder(), builder->get_var_key());
            builder->push_var(ident, var);
            instructions.push_back(Instruction(InstructionType::PUSH, { var->get_key() }));
            if (type == StatementType::VAR_CREATE) {
                Expression expr = expressions.back();
                handle_set(builder, &instructions, var, expr);
            } else if (type == StatementType::FUNCTION_CREATE) {
                // // instructions.push_back(Instruction(InstructionType::SET, { frame_negate(builder->get_frame()), 0 }));
                // Scope* scope = new Scope(ScopeType::FUNC, builder->get_frame() + 1);
                // builder->push_scope(scope);
                // Statement sub_statement = statements.back();
                // std::vector<Instruction> sub_instructions = sub_statement.generate_instructions(builder);
                // // instructions.push_back(Instruction(InstructionType::SKIP, { (int)sub_instructions.size() + 2 }));
                // merge(&instructions, sub_instructions);
                // // instructions.push_back(Instruction(InstructionType::BACK, {}));
                // // instructions.push_back(Instruction(InstructionType::EXIT, {}));
                // delete builder->pop_scope();
            }
        } break;
        case StatementType::COMPOUND: {
            for (Statement sub_statement : statements) {
                merge(&instructions, sub_statement.generate_instructions(builder));
            }
        } break;
        case StatementType::ADD_SET:
        case StatementType::SUBTRACT_SET:
        case StatementType::MULTIPLY_SET:
        case StatementType::DIVIDE_SET:
        case StatementType::SET: {
            Expression expr = expressions.front();
            Expression expr_from = expressions[1];
            merge(&instructions, expr.generate_push_instructions(builder));
            Var* var = expr.get_var(builder);
            if (type == StatementType::SET) {
                handle_set(builder, &instructions, var, expr_from);
            } else {
                std::vector<uint> args = var->get_args();
                merge(&args, args);
                merge(&args, expr_from.get_args(builder));
                instructions.push_back(Instruction(InstructionType::ADD, args));
            }
            merge(&instructions, expr.generate_pop_instructions(builder));
        } break;
    }
    return instructions;
}