#include "t_cmp.h"
#include <iostream>
#include <string>
#include <vector>
#include <deque>
#include <sstream>
#include <any>
#include <algorithm>

class CompilerException : public std::exception {
    private:
    public:
        CompilerException() {}
        const char* what() const throw() {
            return "Compiler error";
        }
};

void expected(std::string expected, std::string actual) {
    std::cout << "Expected " << expected << " but got " << actual << " instead" << std::endl;
    throw CompilerException();
}

void already_declared(std::string name) {
    std::cout << name << " has already been declared" << std::endl;
    throw CompilerException();
}

void not_declared(std::string name) {
    std::cout << name << " has not been declared" << std::endl;
    throw CompilerException();
}

template<typename T>
void merge(std::vector<T>* first, std::vector<T> second) {
    first->insert(first->end(), second.begin(), second.end());
}

std::vector<int> offset_args(std::vector<int> args, int amount) {
    if (args[0] < 0) {
        args[1] += amount;
    }
    return args;
}

std::any parse_val(std::string literal, toast::StateTypeHolder type) {
    switch (type.get_main_type()) {
        case toast::INT: {
            std::stringstream stream(literal);
            int val = 0;
            stream >> val;
            if (val == 0 && literal != "0") {
                expected("a number", literal);
            }
            return val;
        } break;
        case toast::BOOL: {
            if (literal != "true" && literal != "false") {
                expected("a boolean", literal);
            }
            return literal == "true";
        } break;
        default:
            std::cout << "Invalid type" << std::endl;
            throw CompilerException();
    }
}

int frame_negate(int frame_key) {
    return (frame_key * -1) - 1;
}

enum TokenType {
    ILLEGAL,
    FILE_END,
    IDENT,
    TYPE_IDENT,
    INT_LITERAL,
    BOOL_LITERAL,
    STR_LITERAL,
    ARRAY_LITERAL,

    IF_WORD,
    ELSE_WORD,
    WHILE_WORD,
    RETURN_WORD,
    DELETE_WORD,

    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_BRACKET,
    RIGHT_BRACKET,
    LEFT_BRACE,
    RIGHT_BRACE,
    LEFT_ANGLE,
    RIGHT_ANGLE,
    COMMA,
    NEW_LINE,
    EQUALS,
    PLUS,
    MINUS,
    TIMES,
    OVER,
    PLUS_EQUALS,
    MINUS_EQUALS,
    TIMES_EQUALS,
    OVER_EQUALS,
    DOUBLE_PLUS,
    DOUBLE_MINUS,
    DOUBLE_EQUALS,
    EXCLAMATION_EQUALS,
    DOUBLE_AMPERSAND,
    DOUBLE_VERT_BAR,
    EXCLAMATION,
    HASH
};

class Token {
    private:
        TokenType type;
        std::string literal;
    public:
        Token(TokenType type, std::string literal) {
            this->type = type;
            this->literal = literal;
        }
        TokenType get_type() {
            return type;
        }
        std::string get_literal() {
            return literal;
        }
        bool is_end() {
            return get_type() == NEW_LINE || get_type() == FILE_END || get_type() == ILLEGAL;
        }
};

class Lexer {
    private:
        int position = 0;
        std::string source;
        std::deque<Token> tokens;
        void handle_char() {
            char ch = source[position];
            char next_ch = ' ';
            if ((position + 1) < source.size()) {
                next_ch = source[position + 1];
            }
            std::string literal(1, ch);
            TokenType type = ILLEGAL;
            switch (ch) {
                case ' ':
                    return;
                case ';':
                case '\n':
                    type = NEW_LINE;
                    break;
                case '\u0000':
                    literal = "";
                    type = FILE_END;
                    break;
                // small tokens
                case '(':
                    type = LEFT_PAREN;
                    break;
                case ')':
                    type = RIGHT_PAREN;
                    break;
                case '[':
                    type = LEFT_BRACKET;
                    break;
                case ']':
                    type = RIGHT_BRACKET;
                    break;
                case '{':
                    type = LEFT_BRACE;
                    break;
                case '}':
                    type = RIGHT_BRACE;
                    break;
                case ',':
                    type = COMMA;
                    break;
                case '!':
                    if (next_ch == '=') {
                        literal = "!=";
                        type = EXCLAMATION_EQUALS;
                        position++;
                        break;
                    }
                    type = EXCLAMATION;
                    break;
                case '#':
                    type = HASH;
                    break;
                case '<':
                    type = LEFT_ANGLE;
                    break;
                case '>':
                    type = RIGHT_ANGLE;
                    break;
                case '=':
                    if (next_ch == '=') {
                        literal = "==";
                        type = DOUBLE_EQUALS;
                        position++;
                        break;
                    }
                    type = EQUALS;
                    break;
                case '+':
                    if (next_ch == '=') {
                        literal = "+=";
                        type = PLUS_EQUALS;
                        position++;
                        break;
                    }
                    if (next_ch == '+') {
                        literal == "++";
                        type = DOUBLE_PLUS;
                        position++;
                        break;
                    }
                    type = PLUS;
                    break;
                case '-':
                    if (next_ch == '=') {
                        literal = "-=";
                        type = MINUS_EQUALS;
                        position++;
                        break;
                    }
                    if (next_ch == '-') {
                        literal == "--";
                        type = DOUBLE_MINUS;
                        position++;
                        break;
                    }
                    type = MINUS;
                    break;
                case '*':
                    if (next_ch == '=') {
                        literal = "*=";
                        type = TIMES_EQUALS;
                        position++;
                        break;
                    }
                    type = TIMES;
                    break;
                case '/':
                    if (next_ch == '=') {
                        literal = "/=";
                        type = OVER_EQUALS;
                        position++;
                        break;
                    }
                    if (next_ch == '/') {
                        // comment time
                        literal = "\n";
                        get_and_move_until_char('\n');
                        type = NEW_LINE;
                        break;
                    }
                    type = OVER;
                    break;
                case '&':
                    if (next_ch == '&') {
                        literal = "&&";
                        type = DOUBLE_AMPERSAND;
                        position++;
                        break;
                    }
                    break;
                case '|':
                    if (next_ch == '|') {
                        literal = "||";
                        type = DOUBLE_VERT_BAR;
                        position++;
                        break;
                    }
                    break;
                case '"':
                case '\'': {
                    type = STR_LITERAL;
                    position++;
                    literal = get_and_move_until_char(ch);
                    break;
                } break;
                default: {
                    if (is_ident(ch)) {
                        // check if we arent already in a keyword
                        if ((position - 1) >= 0 && is_ident(source[position - 1])) {
                            return;
                        }
                        literal = get_source_until_non_letter();
                        if (literal == "int" || literal == "bool" || literal == "string" || literal == "void" || literal == "array" || literal == "function") {
                            type = TYPE_IDENT;
                        } else if (literal == "true" || literal == "false") {
                            type = BOOL_LITERAL;
                        } else if (literal == "return") {
                            type = RETURN_WORD;
                        } else if (literal == "if") {
                            type = IF_WORD;
                        } else if (literal == "else") {
                            type = ELSE_WORD;
                        } else if (literal == "while") {
                            type = WHILE_WORD;
                        } else if (literal == "delete") {
                            type = DELETE_WORD;
                        } else {
                            type = IDENT;
                        }
                    } else if (isdigit(ch)) {
                        // check if we arent already in an int
                        if ((position - 1) >= 0 && isdigit(source[position - 1])) {
                            return;
                        }
                        literal = get_source_until_non_digit();
                        type = INT_LITERAL;
                    }
                } break;
            }
            if (type == ILLEGAL) {
                std::cout << "Illegal token " << literal << "at: " << position << std::endl;
                throw CompilerException();
            }
            Token token = Token(type, literal);
            tokens.push_back(token);
        }
        std::string get_source_until_non_letter() {
            std::stringstream stream;
            for (int i = position; i < source.length(); i++) {
                char ch = source[i];
                if (!is_ident(ch)) {
                    break;
                }
                stream << ch;
            }
            return stream.str();
        }
        std::string get_source_until_non_digit() {
            std::stringstream stream;
            for (int i = position; i < source.length(); i++) {
                char ch = source[i];
                if (!isdigit(ch)) {
                    break;
                }
                stream << ch;
            }
            return stream.str();
        }
        std::string get_and_move_until_char(char until) {
            std::stringstream stream;
            for (int i = position; i < source.length(); i++) {
                char ch = source[i];
                if (ch == until) {
                    position = i;
                    break;
                }
                stream << ch;
                if (i + 1 >= source.length()) {
                    position = i;
                    break;
                }
            }
            return stream.str();
        }
        bool is_ident(char ch) {
            if (isalpha(ch)) {
                return true;
            }
            switch (ch) {
                case '_':
                    return true;
                default:
                    return false;
            }
        }
    public:
        Lexer(std::string source) {
            this->source = source;
            for (position = 0; position < source.length(); position++) {
                handle_char();
            }
            if (tokens.size() > 0) {
                if (tokens.back().get_type() != FILE_END) {
                    tokens.push_back(Token(FILE_END, ""));
                }
            } else {
                tokens.push_back(Token(FILE_END, ""));
            }
        }
        std::deque<Token> get_tokens() {
            return tokens;
        }
};

class State {
    private:
        std::vector<toast::StateTypeHolder> type;
        int stack_frame;
        int offset;
    public:
        State(toast::StateTypeHolder type, int stack_frame) {
            this->type.push_back(type);
            this->stack_frame = stack_frame;
        }
        void set_stack_frame();
        toast::StateTypeHolder get_type() {
            return type.back();
        }
        int get_frame() {
            return stack_frame;
        }
        void set_offset(int offset) {
            this->offset = offset;
        }
        int get_offset() {
            return offset;
        }
        std::vector<int> get_args() {
            return { frame_negate(stack_frame), offset };
        }
};

enum ScopeType {
    GLOBAL,
    FUNC,
    BLOCK
};

class Scope {
    private:
        ScopeType type;
        std::vector<State*> state_stack;
        std::map<std::string, State*> state_map;
        int stack_frame;
    public:
        Scope(ScopeType type, int stack_frame) {
            this->type = type;
            this->stack_frame = stack_frame;
        }
        ~Scope() {
            for (State* state : state_stack) {
                delete state;
            }
        }
        void push(State* state) {
            return state_stack.push_back(state);
        }
        std::vector<State*> get_state_stack() {
            return state_stack;
        }
        void add_state(std::string ident, State* state) {
            state_map[ident] = state;
            state_stack.push_back(state);
        }
        bool has_state(std::string ident) {
            return state_map.count(ident) != 0;
        }
        bool has_state(State* state) {
            // google was real helpful here
            bool val = false;
            std::for_each(state_map.begin(), state_map.end(), [&](const std::pair<std::string, State*> entry) {
                if (entry.second == state) {
                    val = true;
                }
            });
            return val;
        }
        int get_state_offset(std::string ident) {
            State* state = state_map[ident];
            return get_state_offset(state);
        }
        int get_state_offset(State* state) {
            for (int i = 0; i < state_stack.size(); i++) {
                State* check_state = state_stack[i];
                if (state == check_state) {
                    return (state_stack.size() - 1) - i;
                }
            }
            return 0;
        }
        State* get_state(std::string ident) {
            return state_map[ident];
        }
        ScopeType get_type() {
            return type;
        }
        int get_frame() {
            return stack_frame;
        }
        std::vector<toast::Instruction> get_instructions() {
            std::vector<toast::Instruction> instructions;
            for (State* state : state_stack) {
                instructions.push_back(toast::Instruction(toast::POP, {}));
            }
            return instructions;
        }
};

class Script;

class Builder {
    private:
        Script* script;
        std::vector<toast::Instruction> instructions;
        std::vector<Scope*> scope_stack;
        int stack_frame = 0;
        int temp_offset = 0;
    public:
        Builder(Script* script);
        ~Builder() {
            // for (Token token : tokens) {
            //     delete token;
            // }
            for (Scope* scope : scope_stack) {
                delete scope;
            }

        }
        std::vector<toast::Instruction> get_instructions() {
            return instructions;
        }
        bool has_state(std::string ident) {
            for (int i = scope_stack.size() - 1; i >= 0; i--) {
                Scope* scope = scope_stack[i];
                if (scope->has_state(ident)) {
                    return true;
                }
            }
            return false;
        };
        int get_state_offset(State* state) {
            int offset = temp_offset;
            for (int i = scope_stack.size() - 1; i >= 0; i--) {
                Scope* scope = scope_stack[i];
                if (scope->has_state(state)) {
                    return scope->get_state_offset(state) + offset;
                }
                if (scope->get_frame() == state->get_frame()) {
                    offset += scope->get_state_stack().size();
                }
            }
            throw CompilerException();
        }
        State* get_state(std::string ident) {
            for (int i = scope_stack.size() - 1; i >= 0; i--) {
                Scope* scope = scope_stack[i];
                if (scope->has_state(ident)) {
                    State* state = scope->get_state(ident);
                    update_state(state);
                    return state;
                }
            }
            throw CompilerException();
        }
        int get_frame() {
            return scope_stack.back()->get_frame();
        }
        void add_state(std::string ident, State* state) {
            scope_stack.back()->add_state(ident, state);
        }
        void update_state(State* state) {
            state->set_offset(get_state_offset(state));
        }
        void add_temp_offset() {
            temp_offset++;
        }
        void sub_temp_offset() {
            temp_offset--;
        }
        void add_scope(Scope* scope) {
            scope_stack.push_back(scope);
        }
};

enum ScriptType {
    RUNNABLE
};

enum StatementType {
    COMPOUND,
    IF,
    ELSE,
    ELSE_IF,
    WHILE,
    RETURN,
    DELETE,
    FUNCTION_DECLARE,
    FUNCTION_CREATE,
    VAR_DECLARE,
    VAR_CREATE,
    SET,
    STREAM_INTO,
    STREAM_OUT,
    ADD_SET,
    SUBTRACT_SET,
    MULTIPLY_SET,
    DIVIDE_SET,
    INCREMENT,
    DECREMENT,
    EMPTY,
    IGNORE
};

enum ExpressionType {
    INT,
    BOOL,
    STRING,
    ARRAY,
    IDENTIFIER,
    PAREN,
    ARRAY_INDEX,
    FUNCTION_CALL,
    FUNCTION,
    ADD,
    SUBTRACT,
    MULTIPLY,
    DIVIDE,
    IS,
    NOT_IS,
    AND,
    OR,
    NOT,
    LENGTH,
    EX_IGNORE
};

class TypeExpression;
class Expression;
class Statement {
    private:
        StatementType type;
        std::vector<Statement> statements;
        std::vector<TypeExpression> type_expressions;
        std::vector<Expression> expressions;
        std::vector<std::string> identifiers;
    public:
        Statement(std::deque<Token>* tokens);
        StatementType get_type();
        std::vector<Statement> get_statements();
        std::vector<TypeExpression> get_type_expressions();
        std::vector<Expression> get_expressions();
        std::vector<std::string> get_identifiers();
        std::vector<toast::Instruction> generate_instructions(Builder* builder);
        void clean();
};

class TypeExpression {
    private:
        toast::StateType type;
        std::vector<TypeExpression> type_expressions;
    public:
        TypeExpression(toast::StateType type) {
            this->type = type;
        }
        TypeExpression(std::deque<Token>* tokens) {
            Token token = tokens->front();
            std::string literal = token.get_literal();
            if (literal == "int") {
                type = toast::INT;
            } else if (literal == "bool") {
                type = toast::BOOL;
            } else if (literal == "string") {
                type = toast::STRING;
            } else if (literal == "float") {
                type = toast::FLOAT;
            } else if (literal == "function") {
                type = toast::FUNC;
            } else if (literal == "array") {
                type = toast::ARRAY;
            }

            toast::StateTypeHolder cur_type = get_type();

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
                    type = toast::ARRAY;
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
        toast::StateType get_type() {
            return type;
        }
        std::vector<TypeExpression> get_type_expressions() {
            return type_expressions;
        }
        std::vector<int> get_args() {
            switch (type) {
                case toast::INT:
                case toast::BOOL:
                case toast::STRING:
                case toast::FLOAT:
                    return { type };
                default:
                    std::vector<int> args = { type };
                    for (TypeExpression type_expr : type_expressions) {
                        std::vector<int> sub_args = type_expr.get_args();
                        args.push_back(sub_args.size());
                        for (int sub_arg : sub_args) {
                            args.push_back(sub_arg);
                        }
                    }
                    return args;
            }
        }
        toast::StateTypeHolder get_type_holder() {
            switch (type) {
                case toast::INT:
                case toast::BOOL:
                case toast::STRING:
                case toast::FLOAT:
                    return toast::StateTypeHolder(type);
                default:
                    std::vector<toast::StateTypeHolder> sub_types = {};
                    for (TypeExpression type_expr : type_expressions) {
                        sub_types.push_back(type_expr.get_type_holder());
                    }
                    return toast::StateTypeHolder(type, sub_types);
            }
        }
        void add_expression(TypeExpression expr) {
            type_expressions.push_back(expr);
        }
};


class Expression {
    private:
        ExpressionType type;
        std::vector<Statement> statements;
        std::vector<Expression> expressions;
        std::vector<TypeExpression> type_expressions;
        std::vector<std::string> identifiers;
        std::vector<std::any> values;
        void parse_middle(std::deque<Token>* tokens) {
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
                    type = ADD;
                    break;
                case MINUS:
                    type = SUBTRACT;
                    break;
                case TIMES:
                    type = MULTIPLY;
                    break;
                case OVER:
                    type = DIVIDE;
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
    public:
        Expression(ExpressionType type, std::vector<Statement> statements, std::vector<Expression> expressions, std::vector<TypeExpression> type_expressions, std::vector<std::string> identifiers, std::vector<std::any> values) {
            this->type = type;
            this->statements = statements;
            this->expressions = expressions;
            this->type_expressions = type_expressions;
            this->identifiers = identifiers;
            this->values = values;
        }
        Expression(std::deque<Token>* tokens) {
            Token token = tokens->front();
            switch (token.get_type()) {
                // 10 20 34 193 etc
                case INT_LITERAL: {
                    type = INT;
                    values.push_back(parse_val(token.get_literal(), toast::INT));
                    tokens->pop_front();
                } break;
                // true false
                case BOOL_LITERAL: {
                    type = BOOL;
                    values.push_back(parse_val(token.get_literal(), toast::BOOL));
                    tokens->pop_front();
                } break;
                // "Hello world!"
                case STR_LITERAL: {
                    type = STRING;
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
                    type = ARRAY;
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
                    type = LENGTH;
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
        ExpressionType get_type() {
            return type;
        }
        std::vector<Statement> get_statements() {
            return statements;
        }
        std::vector<Expression> get_expressions() {
            return expressions;
        }
        std::vector<TypeExpression> get_type_expressions() {
            return type_expressions;
        }
        std::vector<std::string> get_identifiers() {
            return identifiers;
        }
        std::vector<std::any> get_values() {
            return values;
        }
        void clean() {
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
        bool can_be_set() {
            switch (type) {
                case INT:
                case BOOL:
                case STRING:
                case IDENTIFIER:
                    return true;
                default:
                    return false;
            }
        }
        int get_value() {
            return std::any_cast<int>(values.back());
        }
        std::string get_string_value() {
            return std::any_cast<std::string>(values.back());
        }
        std::vector<toast::Instruction> generate_push_instructions(Builder* builder) {
            std::vector<toast::Instruction> instructions;
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
                    instructions.push_back(toast::Instruction(toast::PUSH, { toast::INT }));
                    toast::InstructionType instruction_type;
                    switch (type) {
                        case ADD:
                            instruction_type = toast::ADD;
                            break;
                        case SUBTRACT:
                            instruction_type = toast::SUBTRACT;
                            break;
                        case MULTIPLY:
                            instruction_type = toast::MULTIPLY;
                            break;
                        case DIVIDE:
                            instruction_type = toast::DIVIDE;
                            break;
                    }
                    std::vector<int> args = get_move_args(builder);
                    std::vector<int> args_1 = expr_1.get_move_args(builder);
                    std::vector<int> args_2 = expr_2.get_move_args(builder);
                    args_1 = offset_args(args_1, 1);
                    args_2 = offset_args(args_2, 1);
                    merge(&args, args_1);
                    merge(&args, args_2);
                    instructions.push_back(toast::Instruction(instruction_type, args));
                } break;
            }
            return instructions;
        }
        std::vector<toast::Instruction> generate_pop_instructions(Builder* builder) {
            std::vector<toast::Instruction> instructions;
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
                    instructions.push_back(toast::Instruction(toast::POP, {}));
                } break;
            }
            return instructions;
        }
        std::vector<int> get_move_args(Builder* builder) {
            switch (type) {
                case INT:
                    return { toast::INT, get_value() };
                case BOOL:
                    return { toast::BOOL, get_value() };
                case IDENTIFIER: {
                    std::string ident = identifiers.back();
                    State* state = builder->get_state(ident);
                    return { frame_negate(state->get_frame()), state->get_offset() };
                } break;
                default:
                    return { frame_negate(builder->get_frame()), 0 };
            }
        }
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
                TypeExpression func_expr = TypeExpression(toast::FUNC);
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

std::vector<toast::Instruction> Statement::generate_instructions(Builder* builder) {
    std::vector<toast::Instruction> instructions;
    switch (type) {
        case VAR_CREATE:
        case VAR_DECLARE:
        case FUNCTION_CREATE:
        case FUNCTION_DECLARE: {
            TypeExpression type_expr = type_expressions.back();
            instructions.push_back(toast::Instruction(toast::PUSH, type_expr.get_args()));
            std::string ident = identifiers.back();
            if (builder->has_state(ident)) {
                already_declared(ident);
            }
            builder->add_state(ident, new State(type_expr.get_type_holder(), builder->get_frame()));
            State* state = builder->get_state(ident);
            if (type == VAR_CREATE) {
                Expression expr = expressions.back();
                if (expr.can_be_set()) {
                    std::vector<int> args = state->get_args();
                    if (state->get_type().equals(toast::STRING)) {
                        instructions.push_back(toast::Instruction(toast::SET, args, expr.get_string_value()));
                    } else {
                        args.push_back(expr.get_value());
                        instructions.push_back(toast::Instruction(toast::SET, args));
                    }
                } else {
                    merge(&instructions, expr.generate_push_instructions(builder));
                    builder->update_state(state);
                    std::vector<int> args = state->get_args();
                    merge(&args, expr.get_move_args(builder));
                    instructions.push_back(toast::Instruction(toast::MOVE, args));
                    merge(&instructions, expr.generate_pop_instructions(builder));
                }
            } else if (type == FUNCTION_CREATE) {
                instructions.push_back(toast::Instruction(toast::SET, { frame_negate(builder->get_frame()), 0 }));
                Scope* scope = new Scope(FUNC, builder->get_frame() + 1);
                builder->add_scope(scope);
                Statement sub_statement = statements.back();
                std::vector<toast::Instruction> sub_instructions = sub_statement.generate_instructions(builder);
                instructions.push_back(toast::Instruction(toast::SKIP, { (int)sub_instructions.size() + 2 }));
                merge(&instructions, sub_instructions);
                instructions.push_back(toast::Instruction(toast::BACK, {}));
                instructions.push_back(toast::Instruction(toast::EXIT, {}));
            }
        } break;
        case COMPOUND: {
            for (Statement sub_statement : statements) {
                merge(&instructions, sub_statement.generate_instructions(builder));
            }
        } break;
    }
    return instructions;
}

class Script {
    private:
        ScriptType type;
        std::vector<Statement> statements;
    public:
        Script(std::deque<Token>* tokens) {
            type = RUNNABLE;
            while (tokens->size() != 0 && tokens->front().get_type() != FILE_END) {
                Token token = tokens->front();
                statements.push_back(Statement(tokens));
            }
        }
        std::vector<Statement> get_statements() {
            return statements;
        }
        void clean() {
            std::vector<Statement> cleaned;
            for (Statement statement : statements) {
                if (statement.get_type() != IGNORE) {
                    statement.clean();
                    cleaned.push_back(statement);
                    std::cout << statement.get_type() << " types: {";
                    for (TypeExpression expression : statement.get_type_expressions()) {
                        std::cout << " " << expression.get_type();
                    }
                    std::cout << " } expressions: {";
                    for (Expression expression : statement.get_expressions()) {
                        std::cout << " " << expression.get_type();
                    }
                    std::cout << " }" << std::endl;
                }
            }
            statements = cleaned;
        }
};

class Parser {
    private:
        Script* script;
    public:
        Parser(std::deque<Token> tokens) {
            script = new Script(&tokens);
            script->clean();
        }
        ~Parser() {

        }
        Script* get_script() {
            return script;
        }
};

Builder::Builder(Script* script) {
    Scope* global_scope = new Scope(GLOBAL, stack_frame);
    add_scope(global_scope);
    for (Statement statement : script->get_statements()) {
        merge(&instructions, statement.generate_instructions(this));
    }
    merge(&instructions, global_scope->get_instructions());
}

std::vector<toast::Instruction> t_cmp::generate_instruction_list(std::string source) {
    try {
        Lexer lexer = Lexer(source);
        std::deque<Token> tokens = lexer.get_tokens();
        // for (Token token : tokens) {
        //     std::cout << token.get_type() << " " << token.get_literal() << std::endl;
        // }
        Parser parser = Parser(tokens);
        Builder builder = Builder(parser.get_script());
        return builder.get_instructions();
        // Builder builder = Builder(tokens);
        // std::vector<toast::Instruction> instructions = builder.get_instructions();
        // return instructions;
    } catch (toast::Exception e) {
        std::cout << std::endl << e.what() << std::endl;
        return {};
    }
}

std::string t_cmp::make_human_readable(std::vector<toast::Instruction> instructions) {
    std::stringstream stream;
    for (int i = 0; i < instructions.size(); i++) {
        stream << toast::make_human_readable(instructions[i]);
    }
    return stream.str();
}

toast::StateTypeHolder get_type(std::string literal) {
    toast::StateType type_enum;
    if (literal == "void") {
        type_enum = toast::VOID;
    } else if (literal == "int") {
        type_enum = toast::INT;
    } else if (literal == "bool") {
        type_enum = toast::BOOL;
    } else {
        throw toast::Exception("Invalid type");
    }
    return toast::StateTypeHolder(type_enum);
}