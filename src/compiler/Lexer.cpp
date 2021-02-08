#include "Lexer.h"
using namespace toast;
using namespace std;

Lexer::Lexer(std::string source) {
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

std::deque<Token> Lexer::get_tokens() {
    return tokens;
}

void Lexer::handle_char() {
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

std::string Lexer::get_source_until_non_letter() {
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

std::string Lexer::get_source_until_non_digit() {
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

std::string Lexer::get_and_move_until_char(char until) {
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

bool Lexer::is_ident(char ch) {
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