#include "t_cmp.h"
#include <iostream>
#include <string>
#include <vector>
#include <sstream>


enum TokenType {
    ILLEGAL,
    FILE_END,
    IDENT,
    TYPE_IDENT,
    INT_LITERAL,
    BOOL_LITERAL,
    STR_LITERAL,
    ARRAY_LITERAL,

    IF,
    ELSE,
    WHILE,
    RETURN,

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
    STREAM_IN,
    STREAM_OUT,
    DOUBLE_EQUALS,
    DOUBLE_AMPERSAND,
    DOUBLE_VERT_BAR,
    EXCLAMATION,
    LENGTH
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
        std::vector<Token> tokens;
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
                    type = LENGTH;
                    break;
                case '<':
                    if (next_ch == '<') {
                        literal = "<<";
                        type = STREAM_IN;
                        position++;
                        break;
                    }
                    type = LEFT_ANGLE;
                    break;
                case '>':
                    if (next_ch == '>') {
                        literal = ">>";
                        type = STREAM_OUT;
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
                            type = RETURN;
                        } else if (literal == "if") {
                            type = IF;
                        } else if (literal == "while") {
                            type = WHILE;
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
                throw toast::Exception("Illegal token");
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
        std::vector<Token> get_tokens() {
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
    FUNCTION,
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


class Builder {
    private:
        std::vector<Token> tokens;
        std::vector<toast::Instruction> instructions;
        std::vector<Scope*> scope_stack;
        int stack_frame = 0;
    public:
        Builder(std::vector<Token> tokens) {
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
        std::vector<Token> tokens = lexer.get_tokens();
        for (Token token : tokens) {
            std::cout << token.get_type() << " " << token.get_literal() << std::endl;
        }
        Builder builder = Builder(tokens);
        std::vector<toast::Instruction> instructions = builder.get_instructions();
        return instructions;
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