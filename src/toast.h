#include <iostream>
#include <exception>

namespace toast {
    typedef unsigned char byte;
    
    class Exception : public std::exception {
        private:
            const char* msg;
        public:
            Exception(const char* msg);
            const char* what() const throw();
    };
}