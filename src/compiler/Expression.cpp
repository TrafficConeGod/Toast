#include "Expression.h"
#include "Token.h"
#include "TokenType.h"
#include "StatementType.h"
#include "Statement.h"
#include "Var.h"
#include "CompilerException.h"
#include "t_cmp.h"
using namespace toast;

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
        case TokenType::INT_LITERAL: {
            type = ExpressionType::INT;
            values.push_back(parse_val(token.get_literal(), StateType::INT));
            tokens->pop_front();
        } break;
        // true false
        case TokenType::BOOL_LITERAL: {
            type = ExpressionType::BOOL;
            values.push_back(parse_val(token.get_literal(), StateType::BOOL));
            tokens->pop_front();
        } break;
        // "Hello world!"
        case TokenType::STR_LITERAL: {
            type = ExpressionType::STRING;
            values.push_back(token.get_literal());
            tokens->pop_front();
        } break;
        // var_name
        case TokenType::IDENT: {
            type = ExpressionType::IDENTIFIER;
            identifiers.push_back(token.get_literal());
            tokens->pop_front();
        } break;
        // (expression)
        case TokenType::LEFT_PAREN: {
            tokens->pop_front();
            Expression expression = Expression(tokens);
            type = expression.get_type();
            statements = expression.get_statements();
            expressions = expression.get_expressions();
            type_expressions = expression.get_type_expressions();
            identifiers = expression.get_identifiers();
            values = expression.get_values();
            Token right_paren = tokens->front();
            if (right_paren.get_type() != TokenType::RIGHT_PAREN) {
                expected(")", right_paren.get_literal());
            }
            tokens->pop_front();
        } break;
        // [expression, expression]
        case TokenType::LEFT_BRACKET: {
            type = ExpressionType::ARRAY;
            tokens->pop_front();
            if (tokens->front().get_type() != TokenType::RIGHT_BRACKET) {
                for (;;) {
                    Token token = tokens->front();
                    expressions.push_back(Expression(tokens));
                    Token next = tokens->front();
                    tokens->pop_front();
                    if (next.get_type() == TokenType::RIGHT_BRACKET) {
                        break;
                    }
                }
            }
        } break;
        // !expression
        case TokenType::EXCLAMATION: {
            type = ExpressionType::NOT;
            tokens->pop_front();
            expressions.push_back(Expression(tokens));
        } break;
        // #expression
        case TokenType::HASH: {
            type = ExpressionType::LENGTH;
            tokens->pop_front();
            expressions.push_back(Expression(tokens));
        } break;
        // type(type identifier) {}
        case TokenType::TYPE_IDENT: {
            type = ExpressionType::FUNCTION;
            type_expressions.push_back(TypeExpression(tokens));
            Token left_paren = tokens->front();
            if (left_paren.get_type() != TokenType::LEFT_PAREN) {
                expected("(", left_paren.get_literal());
            }
            tokens->pop_front();
            if (tokens->front().get_type() != TokenType::RIGHT_PAREN) {
                for (;;) {
                    type_expressions.push_back(TypeExpression(tokens));
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
            statements.push_back(Statement(tokens));
        } break;
        case TokenType::FILE_END:
        case TokenType::NEW_LINE: {
            type = ExpressionType::IGNORE;
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
        case TokenType::LEFT_PAREN:
        case TokenType::PLUS:
        case TokenType::MINUS:
        case TokenType::TIMES:
        case TokenType::OVER:
        case TokenType::DOUBLE_EQUALS:
        case TokenType::EXCLAMATION_EQUALS:
        case TokenType::DOUBLE_AMPERSAND:
        case TokenType::DOUBLE_VERT_BAR:
        case TokenType::LEFT_BRACKET:
            break;
        default:
            return;
    }
    tokens->pop_front();
    ExpressionType old_type = type;
    switch (middle.get_type()) {
        case TokenType::LEFT_PAREN: {
            type = ExpressionType::FUNCTION_CALL;
            if (tokens->front().get_type() != TokenType::RIGHT_PAREN) {
                for (;;) {
                    Token token = tokens->front();
                    expressions.push_back(Expression(tokens));
                    Token next = tokens->front();
                    tokens->pop_front();
                    if (next.get_type() == TokenType::RIGHT_PAREN) {
                        break;
                    }
                }
            }
            parse_middle(tokens);
            return;
        }
        case TokenType::PLUS:
            type = ExpressionType::ADD;
            break;
        case TokenType::MINUS:
            type = ExpressionType::SUBTRACT;
            break;
        case TokenType::TIMES:
            type = ExpressionType::MULTIPLY;
            break;
        case TokenType::OVER:
            type = ExpressionType::DIVIDE;
            break;
        case TokenType::DOUBLE_EQUALS:
            type = ExpressionType::EQUALS;
            break;
        case TokenType::EXCLAMATION_EQUALS:
            type = ExpressionType::NOT_EQUALS;
            break;
        case TokenType::DOUBLE_AMPERSAND:
            type = ExpressionType::AND;
            break;
        case TokenType::DOUBLE_VERT_BAR:
            type = ExpressionType::OR;
            break;
        case TokenType::LEFT_BRACKET:
            type = ExpressionType::ARRAY_INDEX;
            expressions.push_back(Expression(tokens));
            if (tokens->front().get_type() != TokenType::RIGHT_BRACKET) {
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

int Expression::get_value() {
    return std::any_cast<int>(values.back());
}

std::string Expression::get_string_value() {
    return std::any_cast<std::string>(values.back());
}

std::vector<Instruction> Expression::generate_push_instructions(Builder* builder) {
    std::vector<Instruction> instructions;
    switch (type) {
        case ExpressionType::ADD:
        case ExpressionType::SUBTRACT:
        case ExpressionType::MULTIPLY:
        case ExpressionType::DIVIDE: {
            var_key = builder->get_var_key();
            instructions.push_back(Instruction(InstructionType::PUSH, { var_key }));
            Expression expr_1 = expressions[0];
            Expression expr_2 = expressions[1];
            std::vector<uint> args = get_args(builder);
            std::vector<uint> args_1 = expr_1.get_args(builder);
            merge(&instructions, expr_1.generate_push_instructions(builder));
            std::vector<uint> args_2 = expr_2.get_args(builder);
            merge(&instructions, expr_2.generate_push_instructions(builder));
            merge(&args, args_1);
            merge(&args, args_2);
            // builder->add_temp_offset();
            builder->push_var();
            InstructionType instruction_type;
            switch (type) {
                case ExpressionType::ADD:
                    instruction_type = InstructionType::ADD;
                    break;
                case ExpressionType::SUBTRACT:
                    instruction_type = InstructionType::SUBTRACT;
                    break;
                case ExpressionType::MULTIPLY:
                    instruction_type = InstructionType::MULTIPLY;
                    break;
                case ExpressionType::DIVIDE:
                    instruction_type = InstructionType::DIVIDE;
                    break;
            }
            std::vector<State*> states = get_states(builder);
            std::vector<State*> states_1 = expr_1.get_states(builder);
            std::vector<State*> states_2 = expr_2.get_states(builder);
            merge(&states, states_1);
            merge(&states, states_2);
            instructions.push_back(Instruction(instruction_type, args, states));
        } break;
        case ExpressionType::FUNCTION: {
            // builder->add_temp_offset();
            // instructions.push_back(Instruction(InstructionType::PUSH, get_type_holder(builder).get_args()));
            // instructions.push_back(Instruction(InstructionType::SET, get_move_args(builder)));
            Scope* scope = new Scope(ScopeType::FUNC, builder->get_frame() + 1);
            builder->push_scope(scope);
            Statement sub_statement = statements.back();
            std::vector<Instruction> sub_instructions = sub_statement.generate_instructions(builder);
            // instructions.push_back(Instruction(InstructionType::SKIP, { (int)sub_instructions.size() + 2 }));
            merge(&instructions, sub_instructions);
            // instructions.push_back(Instruction(InstructionType::BACK, {}));
            // instructions.push_back(Instruction(InstructionType::EXIT, {}));
            delete builder->pop_scope();
        } break;
    }
    return instructions;
}

std::vector<Instruction> Expression::generate_pop_instructions(Builder* builder) {
    std::vector<Instruction> instructions;
    switch (type) {
        case ExpressionType::ADD:
        case ExpressionType::SUBTRACT:
        case ExpressionType::MULTIPLY:
        case ExpressionType::DIVIDE: {
            Expression expr_1 = expressions[0];
            Expression expr_2 = expressions[1];
            merge(&instructions, expr_1.generate_pop_instructions(builder));
            merge(&instructions, expr_2.generate_pop_instructions(builder));
            // builder->sub_temp_offset();
            instructions.push_back(Instruction(InstructionType::POP, { var_key }));
        } break;
        case ExpressionType::FUNCTION:
            // builder->sub_temp_offset();
            // instructions.push_back(Instruction(InstructionType::POP, {}));
            break;
    }
    return instructions;
}

std::vector<uint> Expression::get_args(Builder* builder) {
    switch (type) {
        case ExpressionType::INT:
        case ExpressionType::BOOL:
            return { 0 };
        case ExpressionType::IDENTIFIER: {
            std::string ident = identifiers.back();
            Var* var = builder->get_var(ident);
            return { var->get_key() };
        } break;
        default:
            return { var_key };
    }
}

std::vector<State*> Expression::get_states(Builder* builder) {
    switch (type) {
        case ExpressionType::INT:
            return { new State(StateType::INT, values.back()) };
        case ExpressionType::BOOL:
            return { new State(StateType::BOOL, values.back()) };
        default:
            return {};
    }
}

StateTypeHolder Expression::get_type_holder(Builder* builder) {
    switch (type) {
        case ExpressionType::INT:
            return StateTypeHolder(StateType::INT);
        case ExpressionType::BOOL:
            return StateTypeHolder(StateType::BOOL);
        case ExpressionType::STRING:
            return StateTypeHolder(StateType::STRING);
        case ExpressionType::IDENTIFIER: {
            std::string ident = identifiers.back();
            if (!builder->has_var(ident)) {
                not_declared(ident);
            }
            Var* var = builder->get_var(ident);
            return var->get_type();
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

void Expression::check_type(Builder* builder, Var* var) {
    if (!var->get_type().equals(get_type_holder(builder))) {
        std::cout << "Types are incompatible" << std::endl;
        throw CompilerException();
    }
}

Var* Expression::get_var(Builder* builder) {
    switch (type) {
        case ExpressionType::IDENTIFIER: {
            std::string ident = identifiers.back();
            if (!builder->has_var(ident)) {
                not_declared(ident);
            }
            Var* var = builder->get_var(ident);
            return var;
        } break;
        default:
            std::cout << "Not identifiying" << std::endl;
            throw CompilerException();
    }
}

bool Expression::can_be_set() {
    switch (type) {
        case ExpressionType::INT:
        case ExpressionType::BOOL:
        case ExpressionType::STRING:
            return true;
        default:
            return false;
    }
}