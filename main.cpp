#include <iostream>
#include "chip8.h"

Chip8 C8;
int main() {
    C8.initialize();
    C8.loadProgram("C:\\Users\\LENOVO\\CLionProjects\\chip-8\\roms\\IBM Logo.ch8");
    C8.emulateCycle();
    return 0;
}
