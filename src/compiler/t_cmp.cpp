#include "t_cmp.h"
#include "../shared/StateTypeHolder.h"
#include "../shared/StateType.h"
#include "Lexer.h"
#include "Parser.h"
#include "Builder.h"
#include "CompilerException.h"
using namespace toast;

std::vector<Instruction> toast::generate_instruction_list(std::string source) {
    try {
        Lexer lexer = Lexer(source);
        std::deque<Token> tokens = lexer.get_tokens();
        // for (Token token : tokens) {
        //     std::cout << token.get_type() << " " << token.get_literal() << std::endl;
        // }
        Parser parser(tokens);
        Builder builder(parser.get_script());
        return builder.get_instructions();
        // Builder builder = Builder(tokens);
        // std::vector<Instruction> instructions = builder.get_instructions();
        // return instructions;
    } catch (Exception e) {
        std::cout << std::endl << e.what() << std::endl;
        return {};
    }
}

std::any toast::parse_val(std::string literal, StateTypeHolder type) {
    switch (type.get_main_type()) {
        case StateType::INT: {
            std::stringstream stream(literal);
            int val = 0;
            stream >> val;
            if (val == 0 && literal != "0") {
                expected("a number", literal);
            }
            return val;
        } break;
        case StateType::BOOL: {
            if (literal != "true" && literal != "false") {
                expected("a boolean", literal);
            }
            return literal == "true";
        } break;
        default:
            std::cout << "Invalid type" << std::endl;
            throw CompilerException();
    }
}


void toast::expected(std::string expected, std::string actual) {
    std::cout << "Expected " << expected << " but got " << actual << " instead" << std::endl;
    throw CompilerException();
}

void toast::already_declared(std::string name) {
    std::cout << name << " has already been declared" << std::endl;
    throw CompilerException();
}

void toast::not_declared(std::string name) {
    std::cout << name << " has not been declared" << std::endl;
    throw CompilerException();
}


int toast::frame_negate(int frame_key) {
    return (frame_key * -1) - 1;
}

std::string toast::make_human_readable(std::vector<Instruction> instructions) {
    std::stringstream stream;
    for (int i = 0; i < instructions.size(); i++) {
        stream << instructions[i].make_human_readable();
    }
    return stream.str();
}