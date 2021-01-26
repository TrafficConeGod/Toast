#include "toast.h"

toast::Exception::Exception(const char* msg) {
    this->msg = msg;
}

const char* toast::Exception::what() const throw() {
    return msg;
}
