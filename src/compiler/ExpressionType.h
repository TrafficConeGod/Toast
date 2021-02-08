#pragma once

namespace toast {
    enum ExpressionType {
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
        IS,
        NOT_IS,
        AND,
        OR,
        NOT,
        LENGTH,
        EX_IGNORE
    };
}