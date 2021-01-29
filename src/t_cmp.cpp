#include "t_cmp.h"
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

std::vector<toast::Instruction> t_cmp::generate_instruction_list(std::string source) {
    try {
        Lexer lexer = Lexer(source);
        std::vector<Token> tokens = lexer.get_tokens();
        Builder builder = Builder(tokens);
        std::vector<toast::Instruction> instructions = builder.get_instructions();
        return instructions;
    } catch (toast::Exception e) {
        std::cout << std::endl << e.what() << std::endl;
        return {};
    }
}

std::string t_cmp::make_human_readable(std::vector<toast::Instruction> instructions) {
    std::stringstream stream;
    for (int i = 0; i < instructions.size(); i++) {
        stream << toast::make_human_readable(instructions[i]);
    }
    return stream.str();
}

void t_cmp::delete_instruction_list(std::vector<toast::Instruction> instructions) {
    // for (toast::Instruction instruction : instructions) {
    //     delete instruction;
    // }
}

t_cmp::Lexer::Lexer(std::string source) {
    this->source = source;
    for (position = 0; position < source.length(); position++) {
        handle_char();
    }
}

void t_cmp::Lexer::handle_char() {
    char ch = source[position];
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
                if ((position - 1) >= 0 && isalpha(source[position - 1])) {
                    return;
                }
                literal = get_source_until_non_letter();
                if (literal == "int" || literal == "bool" || literal == "void") {
                    type = TYPE;
                } else if (literal == "return") {
                    type = RETURN;
                } else if (literal == "true" || literal == "false") {
                    type = VAL;
                } else {
                    type = IDENT;
                }
            } else if (isdigit(ch)) {
                // check if we arent already in an int
                if ((position - 1) >= 0 && isdigit(source[position - 1])) {
                    return;
                }
                literal = get_source_until_non_digit();
                type = VAL;
            }
    }
    if (type == ILLEGAL) {
        throw toast::Exception("Illegal token");
    }
    Token token = Token(type, literal);
    tokens.push_back(token);
}

std::string t_cmp::Lexer::get_source_until_non_letter() {
    std::string output;
    std::stringstream stream;
    for (int i = position; i < source.length(); i++) {
        char ch = source[i];
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
    for (int i = position; i < source.length(); i++) {
        char ch = source[i];
        if (!isdigit(ch)) {
            break;
        }
        stream << ch;
    }
    stream >> output;
    return output;
}

std::vector<t_cmp::Token> t_cmp::Lexer::get_tokens() {
    return tokens;
}


t_cmp::Token::Token(TokenType type, std::string literal) {
    this->type = type;
    this->literal = literal;
}

t_cmp::TokenType t_cmp::Token::get_type() {
    return type;
}

std::string t_cmp::Token::get_literal() {
    return literal;
}

t_cmp::Builder::Builder(std::vector<Token> tokens) {
    this->tokens = tokens;
    Scope* global_scope = new Scope(GLOBAL, stack_frame);
    scope_stack.push_back(global_scope);
    for (position = 0; position < tokens.size(); position++) {
        handle_token();
    }
}

toast::StateTypeHolder get_type(std::string literal) {
    toast::StateType type_enum;
    if (literal == "void") {
        type_enum = toast::VOID;
    } else if (literal == "int") {
        type_enum = toast::INT;
    } else if (literal == "bool") {
        type_enum = toast::BOOL;
    } else {
        throw toast::Exception("Invalid type");
    }
    return toast::StateTypeHolder(type_enum);
}

void t_cmp::Builder::check_block(int position) {
    if (position >= tokens.size()) {
        throw toast::Exception("There is no next token");
    }
    Token left_bracket = tokens[position];
    if (left_bracket.get_type() == LEFT_BRACE) {
        for (int i = position + 1; i < tokens.size(); i++) {
            if (tokens[i].get_type() == RIGHT_BRACE) {
                return;
            }
        }
        throw toast::Exception("Expected right bracket");
    } else {
        throw toast::Exception("Expected left bracket");
    }
}

void t_cmp::Builder::declare_var(std::string name, toast::StateTypeHolder type) {
    Scope* scope = scope_stack.back();
    if (type.equals(toast::FUNC)) {
        State* state = new State(type, stack_frame);
        scope->push(state);
        scope->add_var(name, state);
        // add instructions
        toast::Instruction push_instruction = toast::Instruction(toast::PUSH, { type.get_main_type() });
        instructions.push_back(push_instruction);
        // add the scope info to the stack
        stack_frame++;
        Scope* new_scope = new Scope(FUNCTION, stack_frame);
        scope_stack.push_back(new_scope);

    } else {
        toast::Instruction push_instruction = toast::Instruction(toast::PUSH, { type.get_main_type() });
        instructions.push_back(push_instruction);

        State* state = new State(type, stack_frame);
        scope->push(state);
        scope->add_var(name, state);
    }
}

void t_cmp::Builder::set_var(std::string name, Token token) {
    if (!has_var(name)) {
        throw toast::Exception("No var with name");
    }
    State* state = get_var(name);
    if (state->get_type().equals(toast::VOID)) {
        throw toast::Exception("Can not set void");
    }
    if (state->get_type().equals(toast::FUNC)) {
        toast::Instruction set_instruction = toast::Instruction(toast::SET, { state->get_stack_frame(), 0 });
        instructions.push_back(set_instruction);
        toast::Instruction skip_instruction = toast::Instruction(toast::SKIP, { });
        instructions.push_back(skip_instruction);
    } else {
        if (token.get_type() == VAL) {
            int parsed_val = t_cmp::parse_val(token.get_literal(), state->get_type());
            toast::Instruction set_instruction = toast::Instruction(toast::SET, { state->get_stack_frame(), 0, parsed_val });
            instructions.push_back(set_instruction);
        } else {
            if (token.get_type() != VAL && token.get_type() != IDENT) {
                throw toast::Exception("Needs to be a proper value");
            }
            std::string name = token.get_literal();
            if (!has_var(name)) {
                throw toast::Exception("No var with name");
            }
            State* var = get_var(name);
            if (!var->get_type().equals(state->get_type())) {
                throw toast::Exception("Needs to be the same type");
            }
            int offset = get_var_offset(name);
            toast::Instruction move_instruction = toast::Instruction(toast::MOVE, { state->get_stack_frame(), 0, var->get_stack_frame(), offset });
            instructions.push_back(move_instruction);
        }
    }
}

void t_cmp::Builder::call_function(std::string name) {
    State* state = get_var(name);
    int offset = get_var_offset(name);
    toast::Instruction frame_instruction = toast::Instruction(toast::FRAME, { state->get_stack_frame() + 1 });
    instructions.push_back(frame_instruction);
    toast::Instruction call_instruction = toast::Instruction(toast::CALL, { state->get_stack_frame(), offset });
    instructions.push_back(call_instruction);
    position += 2;
}

void t_cmp::Builder::handle_token() {
    Token token = tokens[position];
    // InstructionType type = NONE;
    std::vector<int> args;
    TokenType token_type = token.get_type();
    Scope* scope = scope_stack.back();
    // std::cout << token_type << " " << token.get_literal() << std::endl;
    switch (token_type) {
        case LINE_END:
            return;
        case TYPE: {
            // parsing tokens
            if ((position + 1) >= tokens.size()) {
                throw toast::Exception("There is no next token");
            }
            Token ident = tokens[position + 1];
            if (ident.get_type() != IDENT) {
                throw toast::Exception("Next token is not identity token");
            }
            std::string name = ident.get_literal();
            if (has_var(name)) {
                throw toast::Exception("Var has already been declared");
            }
            std::string literal = token.get_literal();

            // check if we can even check if theres a function definition
            if ((position + 4) < tokens.size() && tokens[position + 3].get_type() == RIGHT_PAREN) {
                Token left_paren = tokens[position + 2];
                Token right_paren = tokens[position + 3];
                // check if theres a matching () pair (no arg support yet)
                if (left_paren.get_type() == LEFT_PAREN) {
                    if (right_paren.get_type() != RIGHT_PAREN) {
                        throw toast::Exception("Expected right paren");
                    }
                    // check if this is a valid block
                    position += 4;
                    check_block(position);
                    // create the type
                    toast::StateTypeHolder type = toast::StateTypeHolder(toast::FUNC);
                    type.func_init(get_type(literal), {});
                    // declare the var
                    declare_var(name, type);
                    set_var(name, Token(ILLEGAL, ""));
                }
            } else { // otherwise we are not declaring a function
                toast::StateTypeHolder type = get_type(literal);
                declare_var(name, type);
                if ((position + 3) < tokens.size() && tokens[position + 2].get_type() != LINE_END) {
                    Token assign = tokens[position + 2];
                    Token val = tokens[position + 3];
                    position += 3;
                    if (assign.get_type() != ASSIGN) {
                        throw toast::Exception("Needs to be an assign");
                    }
                    set_var(name, val);
                }
            }
        } break;
        case IDENT: {
            if ((position + 2) >= tokens.size()) {
                throw toast::Exception("There is no next token");
            }
            std::string name = token.get_literal();
            Token next = tokens[position + 1];
            if (next.get_type() == ASSIGN) {
                Token val = tokens[position + 2];
                position += 2;
                set_var(name, val);
                return;
            } else if (next.get_type() == LEFT_PAREN) {
                call_function(name);
                return;
            }
            throw toast::Exception("Expected expression");

        } break;
        case LEFT_BRACE: {
            check_block(position);
            Scope* new_scope = new Scope(BLOCK, stack_frame);
            scope_stack.push_back(new_scope);
        } break;
        case RIGHT_BRACE: {
            if (scope->get_type() == GLOBAL) {
                throw toast::Exception("Can not exit global scope");
            }
            std::vector<State*> state_stack = scope->get_state_stack();
            for (int i = 0; i < state_stack.size(); i++) {
                toast::Instruction pop_instruction = toast::Instruction(toast::POP, { });
                instructions.push_back(pop_instruction);
            }
            if (scope->get_type() == FUNCTION) {
                stack_frame--;
                toast::Instruction back_instruction = toast::Instruction(toast::BACK, { });
                instructions.push_back(back_instruction);
            }
            if (scope->get_type() == FUNCTION) {
                toast::Instruction exit_instruction = toast::Instruction(toast::EXIT, { });
                instructions.push_back(exit_instruction);
                // ugly code incoming
                for (int i = instructions.size() - 1; i >= 0; i--) {
                    toast::Instruction instruction = instructions[i];
                    if (instruction.get_type() == toast::SKIP && instruction.get_args().size() == 0) {
                        toast::Instruction skip_instruction = toast::Instruction(toast::SKIP, { ((int)instructions.size() - 1) - i });
                        instructions[i] = skip_instruction;
                        // delete instruction;
                        break;
                    }
                }
            }
            delete scope_stack.back();
            scope_stack.pop_back();
        } break;
    }
    // switch (token_type) {
    //     case LINE_END:
    //         return;
    //     case TYPE: {
            // // parsing tokens
            // if ((position + 1) >= tokens.size()) {
            //     throw toast::Exception("There is no next token");
            // }
            // Token ident = tokens[position + 1];
            // if (ident.get_type() != IDENT) {
            //     throw toast::Exception("Next token is not identity token");
            // }
            // std::string name = ident.get_literal();
            // if (has_var(name)) {
            //     throw toast::Exception("Var has already been declared");
            // }
            // std::string literal = token.get_literal();

            // // check if we can even check if theres a function definition
            // if ((position + 4) < tokens.size()) {
            //     Token left_paren = tokens[position + 2];
            //     Token right_paren = tokens[position + 3];
            //     // check if theres a matching () pair (no arg support yet)
            //     if (left_paren->get_type() == LEFT_PAREN) {
            //         if (right_paren->get_type() != RIGHT_PAREN) {
            //             throw toast::Exception("Expected right paren");
            //         }
            //         // check if this is a valid block
            //         check_block(position + 4);
    //                 // add the function to the instruction list
    //                 toast::StateTypeHolder type = toast::StateTypeHolder(toast::FUNC);
    //                 type.func_init(get_type(literal), {});
    //                 // add the state info to the stack
    //                 State* state = new State(type, stack_frame);
    //                 scope->push(state);
    //                 scope->add_var(name, state);
    //                 // add instructions
    //                 Instruction push_instruction = new Instruction(PUSH, { type.get_main_type() });
    //                 instructions.push_back(push_instruction);
    //                 Instruction set_instruction = new Instruction(SET, { state->get_stack_frame(), 0 });
    //                 instructions.push_back(set_instruction);
    //                 // add the scope info to the stack
    //                 stack_frame++;
    //                 Scope* new_scope = new Scope(FUNCTION, stack_frame);
    //                 scope_stack.push_back(new_scope);
    //                 // advance ahead to ignore
    //                 position += 4;
    //                 return;
    //             }
    //         }
            
    //         toast::StateTypeHolder type = get_type(literal);
            
    //         Instruction push_instruction = new Instruction(PUSH, { type.get_main_type() });
    //         instructions.push_back(push_instruction);

    //         State* state = new State(type, stack_frame);
    //         scope->push(state);
    //         scope->add_var(name, state);
    //         if ((position + 3) < tokens.size() && tokens[position + 2]->get_type() != LINE_END) {
    //             if (type.equals(toast::VOID)) {
    //                 throw toast::Exception("Can not set void");
    //             }
                // Token assign = tokens[position + 2];
                // Token val = tokens[position + 3];
                // if (assign->get_type() != ASSIGN || (val->get_type() != VAL && val->get_type() != IDENT)) {
                //     throw toast::Exception("Needs to be an assign with a value");
                // }
    //             if (val->get_type() == VAL) {
    //                 int parsed_val = t_cmp::parse_val(val->get_literal(), type);
    //                 Instruction set_instruction = new Instruction(SET, { state->get_stack_frame(), 0, parsed_val });
    //                 instructions.push_back(set_instruction);
    //             } else {
    //                 std::string name = val->get_literal();
    //                 if (!has_var(name)) {
    //                     throw toast::Exception("No var with name");
    //                 }
    //                 State* var = get_var(name);
    //                 if (!var->get_type()->equals(type)) {
    //                     throw toast::Exception("Needs to be the same type");
    //                 }
    //                 int offset = get_var_offset(name);
    //                 Instruction move_instruction = new Instruction(MOVE, { state->get_stack_frame(), 0, var->get_stack_frame(), offset });
    //                 instructions.push_back(move_instruction);
    //             }
    //             position += 3;
    //         } else {
    //             position++;
    //         }
    //     } break;
    //     case IDENT: {
    //         std::string name = token.get_literal();
    //         if (!has_var(name)) {
    //             throw toast::Exception("No var with name");
    //         }
    //         State* state = get_var(name);
    //         if (state->get_type()->equals(toast::VOID)) {
    //             throw toast::Exception("Can not set void");
    //         }
    //         int offset = get_var_offset(name);
    //         if ((position + 2) >= tokens.size()) {
    //             throw toast::Exception("There is no next token");
    //         }
    //         Token next = tokens[position + 1];
    //         if (next->get_type() == ASSIGN) {
    //             Token val = tokens[position + 2];
    //             if (val->get_type() != VAL && val->get_type() != IDENT) {
    //                 throw toast::Exception("Must be assign and value expression");
    //             }
    //             if (val->get_type() == IDENT) {
    //                 std::string var_name = val->get_literal();
    //                 if (!has_var(var_name)) {
    //                     throw toast::Exception("No var with name");
    //                 }
    //                 State* var = get_var(var_name);
    //                 if (!var->get_type()->equals(state->get_type())) {
    //                     throw toast::Exception("Needs to be the same type");
    //                 }
    //                 int var_offset = get_var_offset(var_name);
    //                 Instruction move_instruction = new Instruction(MOVE, { state->get_stack_frame(), offset, var->get_stack_frame(), var_offset });
    //                 instructions.push_back(move_instruction);
    //             } else {
    //                 int parsed_val = t_cmp::parse_val(val->get_literal(), state->get_type());
    //                 Instruction set_instruction = new Instruction(SET, { state->get_stack_frame(), offset, parsed_val });
    //                 instructions.push_back(set_instruction);
    //             }
    //             position += 2;
    //             return;
    //         } else if (next->get_type() == LEFT_PAREN) {
    //             Token right_paren = tokens[position + 2];
    //             if (right_paren->get_type() != RIGHT_PAREN) {
    //                 throw toast::Exception("Expected right paren");
    //             }
    //             Instruction frame_instruction = new Instruction(FRAME, { state->get_stack_frame() + 1 });
    //             instructions.push_back(frame_instruction);
    //             Instruction call_instruction = new Instruction(CALL, { state->get_stack_frame(), offset });
    //             instructions.push_back(call_instruction);
    //             position += 2;
    //             return;
    //         }
    //         throw toast::Exception("Expected expression");
    //     } break;
    //     case LEFT_BRACE: {
    //         check_block(position);
    //         Scope* new_scope = new Scope(BLOCK, stack_frame);
    //         scope_stack.push_back(new_scope);
    //     } break;
    //     case RIGHT_BRACE: {
    //         if (scope->get_type() == GLOBAL) {
    //             throw toast::Exception("Can not exit global scope");
    //         }
    //         std::vector<State*> state_stack = scope->get_state_stack();
    //         for (int i = 0; i < state_stack.size(); i++) {
    //             Instruction pop_instruction = new Instruction(POP, { });
    //             instructions.push_back(pop_instruction);
    //         }
    //         if (scope->get_type() == FUNCTION) {
    //             stack_frame--;
    //             Instruction exit_func_instruction = new Instruction(EXIT_FUNC, { });
    //             instructions.push_back(exit_func_instruction);
    //         }
    //         scope_stack.pop_back();
    //     } break;
    //     default: {

    //     } break;
    // }
}

std::vector<toast::Instruction> t_cmp::Builder::get_instructions() {
    return instructions;
}

t_cmp::State::State(toast::StateTypeHolder type, int stack_frame) {
    this->type.push_back(type);
    this->stack_frame = stack_frame;
}

toast::StateTypeHolder t_cmp::State::get_type() {
    return type.back();
}

int t_cmp::State::get_stack_frame() {
    return stack_frame;
}

int t_cmp::parse_val(std::string literal, toast::StateTypeHolder type) {
    switch (type.get_main_type()) {
        case toast::INT: {
            std::stringstream stream(literal);
            int val = 0;
            stream >> val;
            if (val == 0 && literal != "0") {
                throw toast::Exception("Not a number");
            }
            return val;
        } break;
        case toast::BOOL: {
            if (literal != "true" && literal != "false") {
                throw toast::Exception("Not a bool");
            }
            return literal == "true";
        } break;
        default:
            return 0;
    }
}

t_cmp::Scope::Scope(ScopeType type, int stack_frame) {
    this->type = type;
    this->stack_frame = stack_frame;
}

void t_cmp::Scope::push(State* state) {
    return state_stack.push_back(state);
}

bool t_cmp::Scope::has_var(std::string name) {
    return var_map.count(name) != 0;
}

t_cmp::State* t_cmp::Scope::get_var(std::string name) {
    return var_map[name];
}

int t_cmp::Scope::get_var_offset(std::string name) {
    State* state = var_map[name];
    for (int i = 0; i < state_stack.size(); i++) {
        State* check_state = state_stack[i];
        if (state == check_state) {
            return (state_stack.size() - 1) - i;
        }
    }
    return 0;
}

void t_cmp::Scope::add_var(std::string name, State* state) {
    var_map[name] = state;
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
    State* state = get_var(name);
    int offset = 0;
    for (int i = scope_stack.size() - 1; i >= 0; i--) {
        Scope* scope = scope_stack[i];
        if (scope->has_var(name)) {
            return scope->get_var_offset(name) + offset;
        }
        if (scope->get_stack_frame() == state->get_stack_frame()) {
            offset += scope->get_state_stack().size();
        }
    }
    throw toast::Exception("No var with name through Builder");
}

int t_cmp::Scope::get_stack_frame() {
    return stack_frame;
}

t_cmp::Builder::~Builder() {
    // for (Token token : tokens) {
    //     delete token;
    // }
    for (Scope* scope : scope_stack) {
        delete scope;
    }

}

t_cmp::Scope::~Scope() {
    for (State* state : state_stack) {
        delete state;
    }
}