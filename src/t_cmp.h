#include <iostream>
#include <vector>

namespace t_cmp {
    std::string compile(std::string source);

    enum TokenType {
        ILLEGAL,
        FILEEND,
        IDENT,
        INT,
        ASSIGN,
        PLUS,
        COMMA,
        LINEEND,
        LPAREN,
        RPAREN,
        LBRACE,
        RBRACE,
        FUNC,
        TYPE,
        RET
    };

    class Token {
        private:
            TokenType type;
            std::string literal;
        public:
            Token(TokenType type, std::string literal);
            TokenType get_type();
            std::string get_literal();
    };

    class Lexer {
        private:
            int position = 0;
            std::string source;
            void handle_char();
            std::string get_source_until_non_letter();
            std::string get_source_until_non_digit();
            std::vector<Token*> tokens;
        public:
            Lexer(std::string source);
    };
}