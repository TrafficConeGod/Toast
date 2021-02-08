#include "Exception.h"
using namespace toast;
using namespace std;

Exception::Exception(const char* msg) {
    this->msg = msg;
}

const char* Exception::what() const throw() {
    return msg;
}