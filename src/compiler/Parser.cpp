#include "Parser.h"
using namespace toast;

Parser::Parser(std::deque<Token> tokens) {
    script = new Script(&tokens);
    script->clean();
}

Script* Parser::get_script() {
    return script;
}