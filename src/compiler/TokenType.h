#pragma once

namespace toast {
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
}