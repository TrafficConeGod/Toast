#include "t_cmp.h"
#include <iostream>
#include <string>
#include <sstream>

std::string t_cmp::compile(std::string source) {
    Lexer* lexer = new Lexer(source);
    return "";
}

t_cmp::Lexer::Lexer(std::string source) {
    this->source = source;
    for (this->position = 0; this->position < source.length(); this->position++) {
        handleChar();
    }
}

void t_cmp::Lexer::handleChar() {
    char ch = this->source[this->position];
    std::string literal(1, ch);
    TokenType type = ILLEGAL;
    switch (ch) {
        case ' ':
            return;
        case '=':
            type = ASSIGN;
            break;
        case ';':
        case '\n':
            type = LINEEND;
            break;
        case '(':
            type = LPAREN;
            break;
        case ')':
            type = RPAREN;
            break;
        case ',':
            type = COMMA;
            break;
        case '+':
            type = PLUS;
            break;
        case '{':
            type = LBRACE;
            break;
        case '}':
            type = RBRACE;
            break;
        case '\u0000':
            literal = "";
            type = FILEEND;
            break;
        default:
            if (isalpha(ch)) {
                // check if we arent already in a keyword
                if ((position - 1) >= 0 && isalpha(this->source[position - 1])) {
                    return;
                }
                literal = this->get_source_until_non_letter();
                if (literal == "int" || literal == "bool") {
                    type = TYPE;
                } else {
                    type = IDENT;
                }
            } else if (isdigit(ch)) {
                // check if we arent already in an int
                if ((position - 1) >= 0 && isdigit(this->source[position - 1])) {
                    return;
                }
                literal = this->get_source_until_non_digit();
                type = INT;
            }
    }
    std::cout << literal << std::endl;
    std::cout << type << std::endl;
}

std::string t_cmp::Lexer::get_source_until_non_letter() {
    std::string output;
    std::stringstream stream;
    for (int i = this->position; i < this->source.length(); i++) {
        char ch = this->source[i];
        if (!isalpha(ch)) {
            break;
        }
        stream << ch;
    }
    stream >> output;
    return output;
}

std::string t_cmp::Lexer::get_source_until_non_digit() {
    std::string output;
    std::stringstream stream;
    for (int i = this->position; i < this->source.length(); i++) {
        char ch = this->source[i];
        if (!isdigit(ch)) {
            break;
        }
        stream << ch;
    }
    stream >> output;
    return output;
}