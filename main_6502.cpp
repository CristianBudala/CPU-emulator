#include <stdio.h>
#include <stdlib.h>

using Byte = unsigned char;
using Word = unsigned short;

using u32 = unsigned int;

struct Mem
{
    static constexpr u32 MAX_MEM = 1024 * 64;
    Byte Data[MAX_MEM];

    void Initialise(){
        for(u32 i{}; i < MAX_MEM; i++ )    Data[i] = 0;
    }

    Byte operator[](u32 Address) const{
        return Data[Address];
    }
};

struct CPU
{
    Word PC; // Program counter
    Word SP; // Stack pointer
    Byte A, X, Y; // Registers: Accumulator, X, Y
    
    // Status flags
    Byte C : 1; // Carry
    Byte Z : 1; // Zero
    Byte I : 1; // Interrupt
    Byte D : 1; // Decimal
    Byte B : 1; // Break
    Byte V : 1; // Overflow flag
    Byte N : 1; // Negative flag

    void Reset(Mem& memory){
        PC = 0XFFFC;
        SP = 0X0100;
        D = 0;
        C = Z= I = D = B = V = N = 0;
        A = X = Y = 0;
        memory.Initialise();
    }

    Byte FetchByte(u32& Cycles, Mem& memory){
        Byte data = memory[PC];
        PC++;
        Cycles--;
        return data;
    }

    void Execute(u32 Cycles, Mem& memory){
        while(Cycles) {
            Byte Instruction = FetchByte(Cycles, memory);
        }
    }
};

int main()
{
    Mem mem;
    CPU cpu;
    cpu.Reset(mem);
    cpu.Execute(2, mem);
    return 0;
}
