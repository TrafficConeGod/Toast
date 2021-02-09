#include "Token.h"
using namespace toast;
using namespace std;

Token::Token(TokenType type, std::string literal) {
    this->type = type;
    this->literal = literal;
}

TokenType Token::get_type() {
    return type;
}
std::string Token::get_literal() {
    return literal;
}

bool Token::is_end() {
    return get_type() == TokenType::NEW_LINE || get_type() == TokenType::FILE_END || get_type() == TokenType::ILLEGAL;
}