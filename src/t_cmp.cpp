#include "t_cmp.h"
#include <iostream>
#include <string>
#include <vector>
#include <deque>
#include <sstream>

class CompilerException : public std::exception {
    private:
    public:
        CompilerException() {}
        const char* what() const throw() {
            return "Compiler error";
        }
};

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
    INTO,
    OUTOF,
    DOUBLE_EQUALS,
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
                    type = EXCLAMATION;
                    break;
                case '#':
                    type = HASH;
                    break;
                case '<':
                    if (next_ch == '<') {
                        literal = "<<";
                        type = INTO;
                        position++;
                        break;
                    }
                    type = LEFT_ANGLE;
                    break;
                case '>':
                    if (next_ch == '>') {
                        literal = ">>";
                        type = OUTOF;
                        position++;
                        break;
                    }
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
                        } else if (literal == "while") {
                            type = WHILE_WORD;
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
    WHILE,
    RETURN,
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
    DECREMENT
};

enum ExpressionType {
    INT,
    BOOL,
    STRING,
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
    LENGTH
};

void expected(std::string expected, std::string actual) {
    std::cout << "Expected " << expected << " but got " << actual << " instead" << std::endl;
    throw CompilerException();
}

class Statement;
class Expression;

class Expression {
    private:
        ExpressionType type;
        std::vector<Statement> statements;
    public:
        Expression(std::deque<Token>* tokens) {
            tokens->pop_front();
        }
        ExpressionType get_type() {
            return type;
        }
};

class TypeExpression {
    private:
        std::vector<toast::StateTypeHolder> type;
    public:
        TypeExpression(std::deque<Token>* tokens) {
            Token token = tokens->front();
            std::string literal = token.get_literal();
            type.push_back(toast::StateTypeHolder(toast::INT));
            tokens->pop_front();
        }
        toast::StateTypeHolder get_type() {
            return type.back();
        }
};

class Statement {
    private:
        StatementType type;
        std::vector<Statement> statements;
        std::vector<TypeExpression> type_expressions;
        std::vector<Expression> expressions;
    public:
        Statement(std::deque<Token>* tokens) {
            Token token = tokens->front();
            switch (token.get_type()) {
                // Compound
                case LEFT_BRACE: {
                    type = COMPOUND;
                    do {
                        Token token = tokens->front();
                        statements.push_back(Statement(tokens));
                    } while (tokens->front().get_type() != RIGHT_BRACE);
                } break;
                // If statement
                case IF_WORD: {
                    type = IF;
                    tokens->pop_front();
                    Token left_paren = tokens->front();
                    if (left_paren.get_type() != LEFT_PAREN) {
                        expected("(", left_paren.get_literal());
                    }
                    tokens->pop_front();
                    expressions.push_back(Expression(tokens));
                    Token right_paren = tokens->front();
                    if (right_paren.get_type() != RIGHT_PAREN) {
                        expected(")", right_paren.get_literal());
                    }
                    tokens->pop_front();
                    statements.push_back(Statement(tokens));
                } break;
                // type ident = expression
                case TYPE_IDENT: {
                    type_expressions.push_back(TypeExpression(tokens));
                    Token ident = tokens->front();
                    if (ident.get_type() != IDENT) {
                        expected("identifier", ident.get_literal());
                    }
                    tokens->pop_front();
                    Token equals = tokens->front();
                    if (equals.get_type() == NEW_LINE || equals.get_type() == FILE_END) {
                        type = VAR_DECLARE;
                        break;
                    }
                    type = VAR_CREATE;
                    if (equals.get_type() != EQUALS) {
                        expected("=", equals.get_literal());
                    }
                    tokens->pop_front();
                    Expression expression = Expression(tokens);
                    Token end = tokens->front();
                    if (end.get_type() != NEW_LINE && end.get_type() != FILE_END) {
                        expected("end", end.get_literal());
                    }
                } break;
                // ident = expression OR ident += expression etc
                case IDENT: {

                } break;
                case FILE_END:
                case NEW_LINE: {
                } break;
                default: {
                    expected("statement", token.get_literal());
                } break;
            }
            tokens->pop_front();
        }
        StatementType get_type() {
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
};

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
                Statement statement = statements.back();
                std::cout << statement.get_type() << std::endl;
                for (TypeExpression expression : statement.get_type_expressions()) {
                    std::cout << expression.get_type().get_main_type() << std::endl;
                }
            }
        }
};

class Parser {
    private:
        std::deque<Token> tokens;
    public:
        Parser(std::deque<Token> tokens) {
            this->tokens = tokens;
            Script script = Script(&tokens);
        }
        ~Parser() {

        }
};

class Builder {
    private:
        std::deque<Token> tokens;
        std::vector<toast::Instruction> instructions;
        std::vector<Scope*> scope_stack;
        int stack_frame = 0;
    public:
        Builder(std::deque<Token> tokens) {
            this->tokens = tokens;
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
        return {};
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


int t_cmp::parse_val(std::string literal, toast::StateTypeHolder type) {
    switch (type.get_main_type()) {
        case toast::INT: {
            std::stringstream stream(literal);
            int val = 0;
            stream >> val;
            if (val == 0 && literal != "0") {
                throw toast::Exception("Not a number");
            }
            return val;
        } break;
        case toast::BOOL: {
            if (literal != "true" && literal != "false") {
                throw toast::Exception("Not a bool");
            }
            return literal == "true";
        } break;
        default:
            return 0;
    }
}