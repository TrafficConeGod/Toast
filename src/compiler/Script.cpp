#include "Script.h"
#include "TokenType.h"
using namespace toast;

Script::Script(std::deque<Token>* tokens) {
    type = ScriptType::RUNNABLE;
    while (tokens->size() != 0 && tokens->front().get_type() != TokenType::FILE_END) {
        Token token = tokens->front();
        if (token.get_type() == TokenType::NEW_LINE) {
            tokens->pop_front();
        } else {
            statements.push_back(Statement(tokens));
            Statement statement = statements.back();
            std::cout << (uint)statement.get_type() << " types: {";
            for (TypeExpression expression : statement.get_type_expressions()) {
                std::cout << " " << (uint)expression.get_type();
            }
            std::cout << " } expressions: {";
            for (Expression expression : statement.get_expressions()) {
                std::cout << " " << (uint)expression.get_type();
            }
            std::cout << " }" << std::endl;
        }
    }
    std::cout << "Script parse ended" << std::endl;
}

std::vector<Statement> Script::get_statements() {
    return statements;
}

// void Script::clean() {
//     std::vector<Statement> cleaned;
//     for (Statement statement : statements) {
//         if (statement.get_type() != StatementType::IGNORE) {
//             statement.clean();
//             cleaned.push_back(statement);
//             std::cout << (uint)statement.get_type() << " types: {";
//             for (TypeExpression expression : statement.get_type_expressions()) {
//                 std::cout << " " << (uint)expression.get_type();
//             }
//             std::cout << " } expressions: {";
//             for (Expression expression : statement.get_expressions()) {
//                 std::cout << " " << (uint)expression.get_type();
//             }
//             std::cout << " }" << std::endl;
//         }
//     }
//     statements = cleaned;
// }