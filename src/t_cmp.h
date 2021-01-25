#include <iostream>

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
        TYPE
    };

    class Lexer {
        private:
            int position = 0;
            std::string source;
            void handleChar();
            std::string get_source_until_non_letter();
            std::string get_source_until_non_digit();
        public:
            Lexer(std::string source);
    };
}