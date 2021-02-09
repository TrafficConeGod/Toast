#pragma once

namespace toast {
    enum class StatementType {
        COMPOUND,
        IF,
        ELSE,
        ELSE_IF,
        WHILE,
        RETURN,
        DELETE,
        FUNCTION_DECLARE,
        FUNCTION_CREATE,
        VAR_DECLARE,
        VAR_CREATE,
        SET,
        STREAM_INTO,
        STREAM_OUT,
        ADD_SET,
        SUBTRACT_SET,
        MULTIPLY_SET,
        DIVIDE_SET,
        INCREMENT,
        DECREMENT,
        EMPTY,
        IGNORE
    };
}