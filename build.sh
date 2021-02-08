#!/bin/sh
cd src
g++ -o ../out shared/Exception.cpp shared/Instruction.cpp shared/StateTypeHolder.cpp virtual_machine/Frame.cpp virtual_machine/Stack.cpp virtual_machine/State.cpp virtual_machine/StateArray.cpp virtual_machine/Runner.cpp virtual_machine/t_vm.cpp toaster.cpp -lstdc++ -Wl,--no-as-needed -ldl -std=c++17 -g