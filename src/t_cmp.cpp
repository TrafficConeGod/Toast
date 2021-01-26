#include "t_cmp.h"
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

std::vector<t_cmp::Instruction*> t_cmp::generate_instruction_list(std::string source) {
    Lexer* lexer = new Lexer(source);
    std::vector<Token*> tokens = lexer->get_tokens();
    // for (int i = 0; i < tokens.size(); i++) {
    //     Token* token = tokens[i];
    //     std::cout << token->get_literal() << " " << token->get_type() << std::endl;
    // }
    Builder* builder = new Builder(tokens);
    return builder->get_instructions();
}

std::string t_cmp::make_human_readable(std::vector<t_cmp::Instruction*>) {
    return "";
}

t_cmp::Lexer::Lexer(std::string source) {
    this->source = source;
    for (this->position = 0; this->position < source.length(); this->position++) {
        handle_char();
    }
}

void t_cmp::Lexer::handle_char() {
    char ch = this->source[this->position];
    std::string literal(1, ch);
    TokenType type = ILLEGAL;
    switch (ch) {
        case ' ':
            return;
        case '=':
            type = ASSIGN;
            break;
        case ';':
        case '\n':
            type = LINE_END;
            break;
        case '(':
            type = LEFT_PAREN;
            break;
        case ')':
            type = RIGHT_PAREN;
            break;
        case ',':
            type = COMMA;
            break;
        case '+':
            type = PLUS;
            break;
        case '{':
            type = LEFT_BRACE;
            break;
        case '}':
            type = RIGHT_BRACE;
            break;
        case '\u0000':
            literal = "";
            type = FILE_END;
            break;
        default:
            if (isalpha(ch)) {
                // check if we arent already in a keyword
                if ((position - 1) >= 0 && isalpha(this->source[position - 1])) {
                    return;
                }
                literal = this->get_source_until_non_letter();
                if (literal == "int" || literal == "bool") {
                    type = TYPE;
                } else if (literal == "return") {
                    type = RET;
                } else {
                    type = IDENT;
                }
            } else if (isdigit(ch)) {
                // check if we arent already in an int
                if ((position - 1) >= 0 && isdigit(this->source[position - 1])) {
                    return;
                }
                literal = this->get_source_until_non_digit();
                type = VAL;
            }
    }
    if (type == ILLEGAL) {
        throw toast::Exception("Illegal token");
    }
    Token* token = new Token(type, literal);
    tokens.push_back(token);
}

std::string t_cmp::Lexer::get_source_until_non_letter() {
    std::string output;
    std::stringstream stream;
    for (int i = this->position; i < this->source.length(); i++) {
        char ch = this->source[i];
        if (!isalpha(ch)) {
            break;
        }
        stream << ch;
    }
    stream >> output;
    return output;
}

std::string t_cmp::Lexer::get_source_until_non_digit() {
    std::string output;
    std::stringstream stream;
    for (int i = this->position; i < this->source.length(); i++) {
        char ch = this->source[i];
        if (!isdigit(ch)) {
            break;
        }
        stream << ch;
    }
    stream >> output;
    return output;
}

std::vector<t_cmp::Token*> t_cmp::Lexer::get_tokens() {
    return this->tokens;
}


t_cmp::Token::Token(TokenType type, std::string literal) {
    this->type = type;
    this->literal = literal;
}

t_cmp::TokenType t_cmp::Token::get_type() {
    return this->type;
}

std::string t_cmp::Token::get_literal() {
    return this->literal;
}

t_cmp::Builder::Builder(std::vector<Token*> tokens) {
    this->tokens = tokens;
    for (this->position = 0; this->position < tokens.size(); this->position++) {
        handle_token();
    }
}

void t_cmp::Builder::handle_token() {
    Token* token = this->tokens[position];
    InstructionType type = NONE;
    std::vector<int> args;
    TokenType token_type = token->get_type();
    switch (token->get_type()) {
        case FILE_END:
        case LINE_END:
            return;
        case TYPE:
            {
                if ((position + 1) >= this->tokens.size()) {
                    throw toast::Exception("There is no next token");
                }
                Token* ident = this->tokens[position + 1];
                if (ident->get_type() != IDENT) {
                    throw toast::Exception("Next token is not identity token");
                }
                std::string name = ident->get_literal();
                if (this->vars.count(name) >= 1) {
                    throw toast::Exception("Variable has already been declared");
                }
                type = VAR_DECL;
                std::string literal = token->get_literal();
                ValueType type;
                if (literal == "int") {
                    type = INT;
                } else if (literal == "bool") {
                    type = BOOL;
                }
                args.push_back(type);
                this->vars[name] = new Var(type, var_index);
                var_index++;
            }
            break;
        case IDENT:
            {
                std::string name = token->get_literal();
                if (this->vars.count(name) < 1) {
                    throw toast::Exception("No var with name");
                }
                Var* var = this->vars[name];
                if ((position + 2) >= this->tokens.size()) {
                    throw toast::Exception("There is no next token");
                }
                Token* assign = this->tokens[position + 1];
                Token* val = this->tokens[position + 2];
                if (assign->get_type() != ASSIGN || val->get_type() != VAL) {
                    throw toast::Exception("Must be assign and value expression");
                }
                int parsed_val = t_cmp::parse_val(val->get_literal(), var->get_type());
                position += 2;
                type = SET_VAR;
                args.push_back(var->get_index());
                args.push_back(parsed_val);
            }
        case RET:
            {
                type = EXIT_SCOPE;
                if ((position + 1) >= this->tokens.size()) {
                    args.push_back(0);
                    args.push_back(-1);
                    break;
                }
                Token* val = this->tokens[position + 1];
                if (val->get_type() == VAL) {
                    int parsed_val = t_cmp::parse_val(val->get_literal(), INT);
                    args.push_back(1);
                    args.push_back(parsed_val);
                    position += 1;
                } else if (val->get_type() == IDENT) {
                    std::string name = val->get_literal();
                    if (this->vars.count(name) < 1) {
                        throw toast::Exception("No var with name");
                    }
                    Var* var = this->vars[name];
                    args.push_back(2);
                    args.push_back(var->get_index());
                    position += 1;
                } else {
                    args.push_back(0);
                    args.push_back(-1);
                }
            }
            break;
        default:
            break;
    }
    if (type == NONE) {
        throw toast::Exception("Invalid instruction generated");
    }
    std::cout << type << std::endl;
}

std::vector<t_cmp::Instruction*> t_cmp::Builder::get_instructions() {
    return this->instructions;
}

t_cmp::Var::Var(ValueType type, long index) {
    this->type = type;
    this->index = index;
}

t_cmp::ValueType t_cmp::Var::get_type() {
    return this->type;
}

long t_cmp::Var::get_index() {
    return this->index;
}

int t_cmp::parse_val(std::string literal, ValueType type) {
    switch (type) {
        case INT:
            {
                std::stringstream stream(literal);
                int val = 0;
                stream >> val;
                return val;
            }
        case BOOL:
            return 0;
        default:
            return 0;
    }
}