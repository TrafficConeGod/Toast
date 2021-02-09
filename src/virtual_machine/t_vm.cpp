#include "t_vm.h"
using namespace toast;

void toast::execute(std::vector<Instruction> instructions) {
    // try {
        Runner runner = Runner(instructions);
    // } catch (toast::Exception e) {
        // std::cout << e.what() << std::endl;
    // }
}