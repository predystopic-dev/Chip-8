//
// Created by LENOVO on 12/14/2022.
//
#include <iostream>
#include <string>
#include <fstream>
#include <cstring>

#ifndef CHIP_8_CHIP8_H
#define CHIP_8_CHIP8_H


class Chip8 {
    unsigned short opcode;  // 35 opcodes
    unsigned char memory[4096];
    unsigned V[16];         // V[15] will be carry flag
    unsigned short I;       // Index register
    unsigned short pc;      // Program Counter 0x000 to 0xFFF

    /*
     * 0x000-0x1FF - Chip 8 interpreter (contains font set in emu)
     * 0x050-0x0A0 - Used for the built-in 4x5 pixel font set (0-F)
     * 0x200-0xFFF - Program ROM and work RAM
    */
    unsigned display[64 * 32];  // graphics array[2048] to store 1 or 0 pixel state (b&w graphics)

    unsigned char delay_timer;
    unsigned char sound_timer;  //  buzzer sounds whenever the sound timer reaches zero

    unsigned short stack[16];   // The stack is used to remember the current location before a jump is performed. So anytime you perform a jump or call a subroutine, store the program counter in the stack before proceeding.
    unsigned short stack_pointer;

    unsigned char key[16];
    unsigned char chip8_fontset[80] =
            {
                    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
                    0x20, 0x60, 0x20, 0x20, 0x70, // 1
                    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
                    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
                    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
                    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
                    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
                    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
                    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
                    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
                    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
                    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
                    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
                    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
                    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
                    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
            };
    public:
        void initialize();
        void loadProgram(const char *);
        void emulateCycle();

        void _00E0() {                                  // Clear the display.
            memset(display, 0, sizeof(display));
        };


        /*
         * nnn  =       address or lowest 12 bits of instruction    =       opcode & 0x0FFF
         * nn   =       nibble or lowest 4 bits of instruction      =       opcode & 0x000F
         * x   =       lower 4 bits of high byte of instruction    =       (opcode & 0x0F00) >> 8
         * y   =       upper 4 bits of low byte of instruction     =       (opcode & 0x00F0) >> 4
         * kk   =       byte or lowest 8-bit value of instruction   =       opcode & 0x00FF
        */
        void _1nnn() {                                  // Jump to location nnn.
            pc = opcode & 0x0FFF;                       // To get the nnn value or lowest 12 bit value of instruction
        }
        void _00EE() {                                  // Return from a subroutine.
            --stack_pointer;
            pc = stack[stack_pointer];
        }
        void _2NNN() {                                  // Call subroutine at nnn.
            stack_pointer++;
            stack[stack_pointer] = pc;
            pc = opcode & 0x0FFF;                       // To get the nnn value or lowest 12 bit value of instruction
        }
        void _7XKK() {                                  //  Set Vx = Vx + kk
            int x = (opcode & 0x0F00) >> 8;
            int byte = opcode & 0x00FF;
            V[x] = V[x] + byte;
        }
        void _6XKK() {                                 // The interpreter puts the value kk into register Vx.
            int x = (opcode & 0x0F00) >> 8;
            int byte = opcode & 0x00FF;
            V[x] = byte;
        }
        void _3XKK() {                                 // Skip next instruction if Vx = kk.
            int x = (opcode & 0x0F00) >> 8;
            int byte = opcode & 0x00FF;
            if(V[x] == byte)
                pc = pc + 2;
        }
        void _4XKK() {                                 // Skip next instruction if Vx != kk.
            int x = (opcode & 0x0F00) >> 8;
            int byte = opcode & 0x00FF;
            if(V[x] != byte)
                pc += 2;
        }
        void _5XY0() {                                 // Skip next instruction if Vx = Vy.
            int x = (opcode & 0x0F00) >> 8;
            int y = (opcode & 0x00F0) >> 4;
            if(V[x] == V[y])
                pc += 2;
        }
        void _8XY0() {
            int x = (opcode & 0x0F00) >> 8;
            int y = (opcode & 0x00F0) >> 4;
            V[x] = V[y];
        }
        void _8XY1() {
            int x = (opcode & 0x0F00) >> 8;
            int y = (opcode & 0x00F0) >> 4;
            V[x] = V[x] | V[y];
        }
        void _8XY2() {
            int x = (opcode & 0x0F00) >> 8;
            int y = (opcode & 0x00F0) >> 4;
            V[x] = V[x] & V[y];
        }
        void _8XY3() {
            int x = (opcode & 0x0F00) >> 8;
            int y = (opcode & 0x00F0) >> 4;
            V[x] = V[x] ^ V[y];
        }
        void _8XY4() {
            int x = (opcode & 0x0F00) >> 8;
            int y = (opcode & 0x00F0) >> 4;
            if ((V[x] + V[y] > 0xFF))   V[0xF] = 1;
            else                        V[0XF] = 0;
            V[x] = (V[x] + V[y]) & 0XFF;
        }
        void _8XY5() {
            int x = (opcode & 0x0F00) >> 8;
            int y = (opcode & 0x00F0) >> 4;
            if (V[x] > V[y])    V[0xF] = 1;
            else                V[0xF] = 0;
            V[x] = V[x] - V[y];
        }
        void _8XY6() {
            int x = (opcode & 0x0F00) >> 8;
            int y = (opcode & 0x00F0) >> 4;
            
        }
        void _9XY0() {
            int x = (opcode & 0x0F00) >> 8;
            int y = (opcode & 0x00F0) >> 4;
            if(V[x] != V[y])
                pc += 2;
        }
        void _ANNN() {                                  // Set index register to nnn
            I = opcode & 0x0FFF;                        // To get the nnn value or lowest 12 bit value of instruction
        }
        void _BNNN() {
            pc = opcode & 0x0FFF + V[0];
        }
};

void Chip8::initialize() {
    pc = 0x200;
    opcode = 0;
    I = 0;
    stack_pointer = 0;

    std::fill_n(display, 64 * 32, 0);
    std::fill_n(stack, 16, 0);
    std::fill_n(memory, 4096, 0);
    std::fill_n(V, 16, 0);

    sound_timer = 0;
    delay_timer = 0;

    for(int i = 0; i < 80; ++i)
        memory[0x50 + i] = chip8_fontset[i];
}

void Chip8::loadProgram(char const* argv) {
    /*
    ERROR in Implementation???
    FILE *file = std::fopen(argv, "rb");
    if (file == NULL) {
        printf("ERROR! empty file.");
        exit(1);
    }
    std::fseek(file, 0L, SEEK_END);
    int size = ftell(file);
    std::fseek(file, 0L, SEEK_SET);

    unsigned *buffer = malloc(size+0x200);
    fread(buffer+0x200, size, 1, f);
    fclose(f);
    */

    std::ifstream file(argv, std::ios::binary | std::ios::ate);
    if (file.is_open()) {
        std::streampos size = file.tellg();
        char *buffer = new char[size];

        file.seekg(0, std::ios::beg);
        file.read(buffer, size);
        file.close();

        for(int i = 0; i < size; ++i)
            memory[i + 512] = buffer[i];
        delete[] buffer;
    }
}

void Chip8::emulateCycle() {
    opcode = memory[pc] << 8 | memory[pc+1];
    pc += 2;
}

#endif //CHIP_8_CHIP8_H
