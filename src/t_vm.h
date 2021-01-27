#include <iostream>
#include <vector>
#include "toast.h"

namespace t_inp {

    class State {
        private:
            toast::byte* value;
        public:
            std::string get_cpp_string();
    };

    void execute(std::vector<toast::Instruction*> instructions);
}