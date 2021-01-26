#include "t_cmp.h"
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

std::vector<t_cmp::Instruction*> t_cmp::generate_instruction_list(std::string source) {
    try {
        Lexer* lexer = new Lexer(source);
        std::vector<Token*> tokens = lexer->get_tokens();
        Builder* builder = new Builder(tokens);
        for (int i = 0; i < builder->get_instructions().size(); i++) {
            Instruction* instruction = builder->get_instructions()[i];
            std::cout << instruction->get_type();
            for (int j = 0; j < instruction->get_args().size(); j++) {
                int arg = instruction->get_args()[j];
                std::cout << " " << arg;
            }
            std::cout << std::endl;
        }
        return builder->get_instructions();
    } catch (toast::Exception e) {
        std::cout << std::endl << e.what() << std::endl;
        return {};
    }
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
                    type = RETURN;
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
    Scope* global_scope = new Scope(GLOBAL);
    this->scope_stack.push_back(global_scope);
    for (this->position = 0; this->position < tokens.size(); this->position++) {
        handle_token();
    }
}

void t_cmp::Builder::handle_token() {
    Token* token = this->tokens[position];
    // InstructionType type = NONE;
    std::vector<int> args;
    TokenType token_type = token->get_type();
    Scope* scope = this->scope_stack.back();
    // std::cout << token_type << " " << token->get_literal() << std::endl;
    switch (token_type) {
        case LINE_END:
            return;
        case TYPE: {
            // parsing tokens
            if ((position + 1) >= this->tokens.size()) {
                throw toast::Exception("There is no next token");
            }
            Token* ident = this->tokens[position + 1];
            if (ident->get_type() != IDENT) {
                throw toast::Exception("Next token is not identity token");
            }
            std::string name = ident->get_literal();
            if (this->has_var(name)) {
                throw toast::Exception("Var has already been declared");
            }
            std::string literal = token->get_literal();
            StateType type_enum;
            if (literal == "int") {
                type_enum = INT;
            } else if (literal == "bool") {
                type_enum = BOOL;
            } else {
                throw toast::Exception("Invalid type");
            }
            StateTypeHolder* type_holder = new StateTypeHolder(type_enum);
            
            Instruction* push_instruction = new Instruction(PUSH, { type_enum });
            this->instructions.push_back(push_instruction);

            if ((position + 3) < this->tokens.size() && this->tokens[position + 2]->get_type() != LINE_END) {
                Token* assign = this->tokens[position + 2];
                Token* val = this->tokens[position + 3];
                if (assign->get_type() != ASSIGN || (val->get_type() != VAL && val->get_type() != IDENT)) {
                    throw toast::Exception("Needs to be an assign with a value");
                }
                if (val->get_type() == VAL) {
                    int parsed_val = t_cmp::parse_val(val->get_literal(), type_enum);
                    Instruction* set_instruction = new Instruction(SET, { 0, parsed_val });
                    this->instructions.push_back(set_instruction);
                } else {
                    std::string name = val->get_literal();
                    if (!this->has_var(name)) {
                        throw toast::Exception("No var with name");
                    }
                    int offset = this->get_var_offset(name) + 1;
                    Instruction* set_to_instruction = new Instruction(SET_TO, { 0, offset });
                    this->instructions.push_back(set_to_instruction);
                }
                position += 3;
            } else {
                position++;
            }
            State* state = new State(type_holder);
            scope->push(state);
            scope->add_var(name, state);
        } break;
        case IDENT: {
            std::string name = token->get_literal();
            if (!this->has_var(name)) {
                throw toast::Exception("No var with name");
            }
            State* state = this->get_var(name);
            int offset = this->get_var_offset(name);
            if ((position + 2) >= this->tokens.size()) {
                throw toast::Exception("There is no next token");
            }
            Token* assign = this->tokens[position + 1];
            Token* val = this->tokens[position + 2];
            if (assign->get_type() != ASSIGN || (val->get_type() != VAL && val->get_type() != IDENT)) {
                throw toast::Exception("Must be assign and value expression");
            }
            if (val->get_type() == IDENT) {
                std::string name = val->get_literal();
                if (!this->has_var(name)) {
                    throw toast::Exception("No var with name");
                }
                int offset = this->get_var_offset(name);
                Instruction* set_to_instruction = new Instruction(SET_TO, { 0, offset });
                this->instructions.push_back(set_to_instruction);
            } else {
                int parsed_val = t_cmp::parse_val(val->get_literal(), state->get_type()->get_main_type());
                Instruction* set_instruction = new Instruction(SET, { offset, parsed_val });
                instructions.push_back(set_instruction);
            }
            position += 2;
        } break;
        case LEFT_BRACE: {
            Scope* new_scope = new Scope(BLOCK);
            this->scope_stack.push_back(new_scope);
        } break;
        case RIGHT_BRACE: {
            if (scope->get_type() == GLOBAL) {
                throw toast::Exception("Can not exit global scope");
            }
            std::vector<State*> state_stack = scope->get_state_stack();
            for (int i = 0; i < state_stack.size(); i++) {
                Instruction* pop_instruction = new Instruction(POP, { });
                this->instructions.push_back(pop_instruction);
            }
        } break;
        default: {

        } break;
    }
    // switch (token_type) {
    //     case FILE_END:
    //     case LINE_END:
    //         return;
    //     case TYPE:
    //         {
    //             type = VAR_DECL;
                // if ((position + 1) >= this->tokens.size()) {
                //     throw toast::Exception("There is no next token");
                // }
                // Token* ident = this->tokens[position + 1];
                // if (ident->get_type() != IDENT) {
                //     throw toast::Exception("Next token is not identity token");
                // }
                // std::string name = ident->get_literal();
                // if (this->vars.count(name) >= 1) {
                //     throw toast::Exception("Stateiable has already been declared");
                // }
                // std::string literal = token->get_literal();
                // StateType type_enum;
                // if (literal == "int") {
                //     type_enum = INT;
                // } else if (literal == "bool") {
                //     type_enum = BOOL;
                // }
    //             StateTypeHolder* type_holder = new StateTypeHolder(type_enum);
    //             args.push_back(type_holder->get_main_type());
    //             this->vars[name] = new State(type_holder, this->var_index);
    //             this->var_index++;
    //             instructions.push_back(new Instruction(type, args));
    //         }
    //         break;
    //     case IDENT:
    //         {
    //             type = SET_VAR;
    //             std::string name = token->get_literal();
    //             if (this->vars.count(name) < 1) {
    //                 throw toast::Exception("No var with name");
    //             }
    //             State* var = this->vars[name];
    //             if ((position + 2) >= this->tokens.size()) {
    //                 throw toast::Exception("There is no next token");
    //             }
    //             Token* assign = this->tokens[position + 1];
    //             Token* val = this->tokens[position + 2];
    //             if (assign->get_type() != ASSIGN || (val->get_type() != VAL && val->get_type() != IDENT)) {
    //                 throw toast::Exception("Must be assign and value expression");
    //             }
                // if (val->get_type() == IDENT) {
                //     // int x = y
                //     throw toast::Exception("Not implemented yet");
                // } else {
                //     int parsed_val = t_cmp::parse_val(val->get_literal(), var->get_type()->get_main_type());
                //     position += 2;
                //     args.push_back(var->get_index());
                //     args.push_back(parsed_val);
                // }
    //             instructions.push_back(new Instruction(type, args));
    //         }
    //     // case RET:
    //     //     {
    //     //         type = EXIT_SCOPE;
    //     //         if ((position + 1) >= this->tokens.size()) {
    //     //             args.push_back(0);
    //     //             args.push_back(-1);
    //     //             break;
    //     //         }
    //     //         Token* val = this->tokens[position + 1];
    //     //         if (val->get_type() == VAL) {
    //     //             int parsed_val = t_cmp::parse_val(val->get_literal(), INT);
    //     //             args.push_back(1);
    //     //             args.push_back(parsed_val);
    //     //             position += 1;
    //     //         } else if (val->get_type() == IDENT) {
    //     //             std::string name = val->get_literal();
    //     //             if (this->vars.count(name) < 1) {
    //     //                 throw toast::Exception("No var with name");
    //     //             }
    //     //             State* var = this->vars[name];
    //     //             args.push_back(2);
    //     //             args.push_back(var->get_index());
    //     //             position += 1;
    //     //         } else {
    //     //             args.push_back(0);
    //     //             args.push_back(-1);
    //     //         }
    //     //     }
    //     //     break;
    //     default:
    //         break;
    // }
    // if (type == NONE) {
    //     throw toast::Exception("Invalid instruction generated");
    // }
}

std::vector<t_cmp::Instruction*> t_cmp::Builder::get_instructions() {
    return this->instructions;
}

t_cmp::State::State(StateTypeHolder* type) {
    this->type = type;
}

t_cmp::StateTypeHolder* t_cmp::State::get_type() {
    return this->type;
}

int t_cmp::parse_val(std::string literal, StateType type) {
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


t_cmp::StateTypeHolder::StateTypeHolder(StateType main_type) {
    this->main_type = main_type;
}

void t_cmp::StateTypeHolder::func_init(StateType return_type, std::vector<StateTypeHolder> func_args) {
    this->return_type = return_type;
    this->func_args = func_args;
}

t_cmp::StateType t_cmp::StateTypeHolder::get_main_type() {
    return this->main_type;
}

t_cmp::StateType t_cmp::StateTypeHolder::get_return_type() {
    return this->return_type;
}

std::vector<t_cmp::StateTypeHolder> t_cmp::StateTypeHolder::get_func_args() {
    return this->func_args;
}

t_cmp::Instruction::Instruction(InstructionType type, std::vector<int> args) {
    this->type = type;
    this->args = args;
}

t_cmp::InstructionType t_cmp::Instruction::get_type() {
    return this->type;
};
std::vector<int> t_cmp::Instruction::get_args() {
    return this->args;
};

t_cmp::Scope::Scope(ScopeType type) {
    this->type = type;
}

void t_cmp::Scope::push(State* state) {
    return this->state_stack.push_back(state);
}

bool t_cmp::Scope::has_var(std::string name) {
    return this->var_map.count(name) != 0;
}

t_cmp::State* t_cmp::Scope::get_var(std::string name) {
    return this->var_map[name];
}

int t_cmp::Scope::get_var_offset(std::string name) {
    State* state = this->var_map[name];
    for (int i = 0; i < state_stack.size(); i++) {
        State* check_state = this->state_stack[i];
        if (state == check_state) {
            return (state_stack.size() - 1) - i;
        }
    }
    return 0;
}

void t_cmp::Scope::add_var(std::string name, State* state) {
    this->var_map[name] = state;
}

t_cmp::ScopeType t_cmp::Scope::get_type() {
    return type;
}

std::vector<t_cmp::State*> t_cmp::Scope::get_state_stack() {
    return state_stack;
}

bool t_cmp::Builder::has_var(std::string name) {
    for (int i = scope_stack.size() - 1; i >= 0; i--) {
        Scope* scope = scope_stack[i];
        if (scope->has_var(name)) {
            return true;
        }
    }
    return false;
}

t_cmp::State* t_cmp::Builder::get_var(std::string name) {
    for (int i = scope_stack.size() - 1; i >= 0; i--) {
        Scope* scope = scope_stack[i];
        if (scope->has_var(name)) {
            return scope->get_var(name);
        }
    }
    throw toast::Exception("No var with name through Builder");
}

int t_cmp::Builder::get_var_offset(std::string name) {
    int offset = 0;
    for (int i = scope_stack.size() - 1; i >= 0; i--) {
        Scope* scope = scope_stack[i];
        if (scope->has_var(name)) {
            return scope->get_var_offset(name) + offset;
        }
        offset += scope->get_state_stack().size();
    }
    throw toast::Exception("No var with name through Builder");
}