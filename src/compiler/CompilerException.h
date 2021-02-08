#pragma once
#include <iostream>
#include <exception>

namespace toast {
    class CompilerException : public std::exception {
        public:
            CompilerException() {}
            const char* what() const throw();
    };
}