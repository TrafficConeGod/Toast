#pragma once

namespace toast {
    enum class InstructionType {
        NONE,
        PUSH,
        POP,
        SET,
        MOVE,
        CALL,
        EXIT,
        // FRAME,
        BACKWARD,
        FORWARD,
        ARG,
        RETURN,
        // MOVE_RETURN,
        ADD,
        SUBTRACT,
        MULTIPLY,
        DIVIDE,
        EQUALS,
        IF,
        LENGTH,
        STREAM_IN,
        STREAM_OUT,
        MOVE_IN,
        MOVE_OUT,
        DELETE,
        FUNCTION,
        COMPARE
    };
}