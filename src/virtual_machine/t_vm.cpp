#include "t_vm.h"
using namespace toast;
using namespace std;

void toast::execute(vector<Instruction> instructions) {
    // try {
        Runner runner = Runner(instructions);
    // } catch (toast::Exception e) {
        // cout << e.what() << endl;
    // }
}