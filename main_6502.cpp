#include <stdio.h>
#include <stdlib.h>

using Byte = unsigned char;
using Word = unsigned short;

using u32 = unsigned int;

struct Mem {
    static constexpr u32 MAX_MEM = 1024 * 64;
    Byte Data[MAX_MEM];

    void Initialise() {
        for (u32 i{}; i < MAX_MEM; i++)
            Data[i] = 0;
    }

    // Read 1 byte
    Byte operator[](u32 Address) const {
        return Data[Address];
    }

    // Write 1 byte
    Byte& operator[](u32 Address) {
        return Data[Address];
    }

    // Write 2 bytes
    void WriteWord(Word Value, u32 Address, u32& Cycles){
        Data[Address] = Value & 0xFF;
        Data[Address + 1] = (Value >> 8);
        Cycles -= 2;
    }
};

struct CPU {
    Word PC;       // Program counter
    Word SP;       // Stack pointer
    Byte A, X, Y;  // Registers: Accumulator, X, Y

    // Status flags
    Byte C : 1;  // Carry
    Byte Z : 1;  // Zero
    Byte I : 1;  // Interrupt
    Byte D : 1;  // Decimal
    Byte B : 1;  // Break
    Byte V : 1;  // Overflow flag
    Byte N : 1;  // Negative flag

    void Reset(Mem& memory) {
        PC = 0XFFFC;
        SP = 0X0100;
        D = 0;
        C = Z = I = D = B = V = N = 0;
        A = X = Y = 0;
        memory.Initialise();
    }

    Byte FetchByte(u32& Cycles, Mem& memory) {
        Byte data = memory[PC];
        PC++;
        Cycles--;
        return data;
    }

    Word FetchWord(u32& Cycles, Mem& memory) {
        // 6502 is little endian
        Word data = memory[PC];
        PC++;
        
        data |= (memory[PC] << 8);
        PC++;

        Cycles -= 2;
        return data;
    }

    Byte ReadByte(u32& Cycles, Byte Address, Mem& memory) {
        Byte data = memory[Address];
        Cycles--;
        return data;
    }

    // OPCODES
    static constexpr Byte 
        INS_LDA_IMM = 0xA9,
        INS_LDA_ZP = 0xA5,
        INS_LDA_ZPX = 0xB5,
        INS_JSR = 0X20;

    void LDASetStatus(){
        Z = (A == 0);
        N = (A & 0x1000000) > 0;
    }

    void Execute(u32 Cycles, Mem& memory) {
        while (Cycles) {
            Byte Ins = FetchByte(Cycles, memory);

            switch (Ins) {
                case INS_LDA_IMM: {
                    Byte Value = FetchByte(Cycles, memory);
                    A = Value;
                    LDASetStatus();
                } break;

                case INS_LDA_ZP: {
                    Byte ZeroPageAddr = FetchByte(Cycles, memory);
                    A = ReadByte(Cycles, ZeroPageAddr, memory);
                    LDASetStatus();
                } break;

                case INS_LDA_ZPX: {
                    Byte ZeroPageAddr = FetchByte(Cycles, memory);
                    ZeroPageAddr += X;
                    Cycles--;
                    A = ReadByte(Cycles, ZeroPageAddr, memory);
                    LDASetStatus();
                } break;

                case INS_JSR: {
                    Word SubAddr = FetchWord(Cycles, memory);
                    memory.WriteWord(PC - 1, SP++, Cycles);
                    PC = SubAddr;
                    Cycles--;
                } break;

                default: {
                    printf("Instruction not handled %d", Ins);
                } break;
            }
        }
    }
};

int main() {
    Mem mem;
    CPU cpu;
    cpu.Reset(mem);

    mem[0xFFFC] = CPU::INS_LDA_IMM;
    mem[0xFFD] = 0x42;

    cpu.Execute(2, mem);
    return 0;
}
