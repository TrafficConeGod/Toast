#include "Lexer.h"
#include "CompilerException.h"
#include "TokenType.h"
using namespace toast;
using namespace std;

Lexer::Lexer(std::string source) {
    this->source = source;
    for (position = 0; position < source.length(); position++) {
        handle_char();
    }
    if (tokens.size() > 0) {
        if (tokens.back().get_type() != TokenType::FILE_END) {
            tokens.push_back(Token(TokenType::FILE_END, ""));
        }
    } else {
        tokens.push_back(Token(TokenType::FILE_END, ""));
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
    TokenType type = TokenType::ILLEGAL;
    switch (ch) {
        case ' ':
            return;
        case ';':
        case '\n':
            type = TokenType::NEW_LINE;
            break;
        case '\u0000':
            literal = "";
            type = TokenType::FILE_END;
            break;
        // small tokens
        case '(':
            type = TokenType::LEFT_PAREN;
            break;
        case ')':
            type = TokenType::RIGHT_PAREN;
            break;
        case '[':
            type = TokenType::LEFT_BRACKET;
            break;
        case ']':
            type = TokenType::RIGHT_BRACKET;
            break;
        case '{':
            type = TokenType::LEFT_BRACE;
            break;
        case '}':
            type = TokenType::RIGHT_BRACE;
            break;
        case ',':
            type = TokenType::COMMA;
            break;
        case '!':
            if (next_ch == '=') {
                literal = "!=";
                type = TokenType::EXCLAMATION_EQUALS;
                position++;
                break;
            }
            type = TokenType::EXCLAMATION;
            break;
        case '#':
            type = TokenType::HASH;
            break;
        case '<':
            type = TokenType::LEFT_ANGLE;
            break;
        case '>':
            type = TokenType::RIGHT_ANGLE;
            break;
        case '=':
            if (next_ch == '=') {
                literal = "==";
                type = TokenType::DOUBLE_EQUALS;
                position++;
                break;
            }
            type = TokenType::EQUALS;
            break;
        case '+':
            if (next_ch == '=') {
                literal = "+=";
                type = TokenType::PLUS_EQUALS;
                position++;
                break;
            }
            if (next_ch == '+') {
                literal == "++";
                type = TokenType::DOUBLE_PLUS;
                position++;
                break;
            }
            type = TokenType::PLUS;
            break;
        case '-':
            if (next_ch == '=') {
                literal = "-=";
                type = TokenType::MINUS_EQUALS;
                position++;
                break;
            }
            if (next_ch == '-') {
                literal == "--";
                type = TokenType::DOUBLE_MINUS;
                position++;
                break;
            }
            type = TokenType::MINUS;
            break;
        case '*':
            if (next_ch == '=') {
                literal = "*=";
                type = TokenType::TIMES_EQUALS;
                position++;
                break;
            }
            type = TokenType::TIMES;
            break;
        case '/':
            if (next_ch == '=') {
                literal = "/=";
                type = TokenType::OVER_EQUALS;
                position++;
                break;
            }
            if (next_ch == '/') {
                // comment time
                literal = "\n";
                get_and_move_until_char('\n');
                type = TokenType::NEW_LINE;
                break;
            }
            type = TokenType::OVER;
            break;
        case '&':
            if (next_ch == '&') {
                literal = "&&";
                type = TokenType::DOUBLE_AMPERSAND;
                position++;
                break;
            }
            break;
        case '|':
            if (next_ch == '|') {
                literal = "||";
                type = TokenType::DOUBLE_VERT_BAR;
                position++;
                break;
            }
            break;
        case '"':
        case '\'': {
            type = TokenType::STR_LITERAL;
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
                    type = TokenType::TYPE_IDENT;
                } else if (literal == "true" || literal == "false") {
                    type = TokenType::BOOL_LITERAL;
                } else if (literal == "return") {
                    type = TokenType::RETURN_WORD;
                } else if (literal == "if") {
                    type = TokenType::IF_WORD;
                } else if (literal == "else") {
                    type = TokenType::ELSE_WORD;
                } else if (literal == "while") {
                    type = TokenType::WHILE_WORD;
                } else if (literal == "delete") {
                    type = TokenType::DELETE_WORD;
                } else {
                    type = TokenType::IDENT;
                }
            } else if (isdigit(ch)) {
                // check if we arent already in an int
                if ((position - 1) >= 0 && isdigit(source[position - 1])) {
                    return;
                }
                literal = get_source_until_non_digit();
                type = TokenType::INT_LITERAL;
            }
        } break;
    }
    if (type == TokenType::ILLEGAL) {
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