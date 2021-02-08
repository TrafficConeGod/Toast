#include "Script.h"
using namespace toast;
using namespace std;

Script::Script(std::deque<Token>* tokens) {
    type = RUNNABLE;
    while (tokens->size() != 0 && tokens->front().get_type() != FILE_END) {
        Token token = tokens->front();
        statements.push_back(Statement(tokens));
    }
}

std::vector<Statement> Script::get_statements() {
    return statements;
}

void Statement::clean() {
    std::vector<Statement> cleaned;
    for (Statement statement : statements) {
        if (statement.get_type() != IGNORE) {
            statement.clean();
            cleaned.push_back(statement);
            // std::cout << statement.get_type() << " types: {";
            // for (TypeExpression expression : statement.get_type_expressions()) {
            //     std::cout << " " << expression.get_type();
            // }
            // std::cout << " } expressions: {";
            // for (Expression expression : statement.get_expressions()) {
            //     std::cout << " " << expression.get_type();
            // }
            // std::cout << " }" << std::endl;
        }
    }
    statements = cleaned;
}