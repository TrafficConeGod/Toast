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
        handle_char();
    }
    for (int i = 0; i < this->tokens.size(); i++) {
        Token* token = this->tokens[i];
        std::cout << token->get_literal() << " " << token->get_type() << std::endl;
    }
}

void t_cmp::Lexer::handle_char() {
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
                } else if (literal == "return") {
                    type = RET;
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
    Token* token = new Token(type, literal);
    tokens.push_back(token);
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


t_cmp::Token::Token(TokenType type, std::string literal) {
    this->type = type;
    this->literal = literal;
}

t_cmp::TokenType t_cmp::Token::get_type() {
    return this->type;
}

std::string t_cmp::Token::get_literal() {
    return this->literal;
}