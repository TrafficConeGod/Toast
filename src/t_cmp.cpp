#include "t_cmp.h"
#include <iostream>
#include <string>
#include <vector>
#include <deque>
#include <sstream>
#include <any>

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
                        get_and_move_until_char('\n');
                        return;
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
    public:
        State(toast::StateTypeHolder type, int stack_frame) {
            this->type.push_back(type);
            this->stack_frame = stack_frame;
        }
        void set_stack_frame();
        toast::StateTypeHolder get_type() {
            return type.back();
        }
        int get_stack_frame() {
            return stack_frame;
        }
        int get_stack_frame_in_instruction() {
            return (stack_frame * -1) - 1;
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
        std::map<std::string, State*> var_map;
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
        void add_var(std::string name, State* state) {
            var_map[name] = state;
        }
        bool has_var(std::string name) {
            return var_map.count(name) != 0;
        }
        int get_var_offset(std::string name) {
            State* state = var_map[name];
            for (int i = 0; i < state_stack.size(); i++) {
                State* check_state = state_stack[i];
                if (state == check_state) {
                    return (state_stack.size() - 1) - i;
                }
            }
            return 0;
        }
        State* get_var(std::string name) {
            return var_map[name];
        }
        ScopeType get_type() {
            return type;
        }
        int get_stack_frame() {
            return stack_frame;
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
    VAR_SET,
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
            if (next_type == IDENT || next_type == RIGHT_ANGLE) {
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
                    if (tokens->front().get_type() != RIGHT_BRACKET) {
                        expected("]", tokens->front().get_literal());
                    }
                    type = ARRAY_INDEX;
                    break;
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
                    type = VAR_SET;
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

class Builder {
    private:
        Script* script;
        std::vector<toast::Instruction> instructions;
        std::vector<Scope*> scope_stack;
        int stack_frame = 0;
    public:
        Builder(Script* script) {
            Scope* global_scope = new Scope(GLOBAL, stack_frame);
            scope_stack.push_back(global_scope);
            
        }
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

};

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
