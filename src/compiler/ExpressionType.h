#pragma once

namespace toast {
    enum class ExpressionType {
        INT,
        BOOL,
        STRING,
        ARRAY,
        IDENTIFIER,
        PAREN,
        ARRAY_INDEX,
        FUNCTION_CALL,
        FUNCTION,
        ADD,
        SUBTRACT,
        MULTIPLY,
        DIVIDE,
        EQUALS,
        NOT_EQUALS,
        AND,
        OR,
        NOT,
        LENGTH,
        LESS_THAN,
        GREATER_THAN,
    };
}