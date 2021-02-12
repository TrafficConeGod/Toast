#include "Var.h"
#include "t_cmp.h"
using namespace toast;

Var::Var(StateTypeHolder type, uint key) {
    this->type.push_back(type);
    this->key = key;
}

StateTypeHolder Var::get_type() {
    return type.back();
}

uint Var::get_key() {
    return key;
}

std::vector<uint> Var::get_args() {
    return { key };
}