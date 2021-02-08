#include "CompilerException.h"
using namespace toast;

const char* CompilerException::what() const throw() {
    return "Compiler error";
}