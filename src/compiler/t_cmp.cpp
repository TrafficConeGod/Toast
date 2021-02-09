#include "t_cmp.h"
#include "../shared/StateTypeHolder.h"
#include "../shared/StateType.h"
#include "Lexer.h"
#include "Parser.h"
#include "Builder.h"
#include "CompilerException.h"
using namespace toast;
using namespace std;

vector<Instruction> toast::generate_instruction_list(string source) {
    try {
        Lexer lexer = Lexer(source);
        deque<Token> tokens = lexer.get_tokens();
        // for (Token token : tokens) {
        //     cout << token.get_type() << " " << token.get_literal() << endl;
        // }
        Parser parser = Parser(tokens);
        Builder builder = Builder(parser.get_script());
        return builder.get_instructions();
        // Builder builder = Builder(tokens);
        // vector<Instruction> instructions = builder.get_instructions();
        // return instructions;
    } catch (Exception e) {
        cout << endl << e.what() << endl;
        return {};
    }
}

any toast::parse_val(string literal, StateTypeHolder type) {
    switch (type.get_main_type()) {
        case StateType::INT: {
            stringstream stream(literal);
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
            cout << "Invalid type" << endl;
            throw CompilerException();
    }
}


void toast::expected(string expected, string actual) {
    cout << "Expected " << expected << " but got " << actual << " instead" << endl;
    throw CompilerException();
}

void toast::already_declared(string name) {
    cout << name << " has already been declared" << endl;
    throw CompilerException();
}

void toast::not_declared(string name) {
    cout << name << " has not been declared" << endl;
    throw CompilerException();
}

vector<int> toast::offset_args(vector<int> args, int amount) {
    if (args[0] < 0) {
        args[1] += amount;
    }
    return args;
}

int toast::frame_negate(int frame_key) {
    return (frame_key * -1) - 1;
}

std::string toast::make_human_readable(std::vector<toast::Instruction> instructions) {
    std::stringstream stream;
    for (int i = 0; i < instructions.size(); i++) {
        stream << instructions[i].make_human_readable();
    }
    return stream.str();
}