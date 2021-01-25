#include <iostream>
#include <exception>

namespace toast {
    class Exception : public std::exception {
        private:
            const char* msg;
        public:
            Exception(const char* msg);
            const char* what() const throw();
    };
}