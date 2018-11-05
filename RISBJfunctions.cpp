#include <iostream>
#include <stdint.h>
using namespace std;

int R(uint32_t instruction){
    char opcode = instruction & 0x0000007f; //removes all but last 7 bit, 7f = 0111 1111
    char rd = (instruction >> 7) & 0x0000001f; //shifts down to rd in bottom 5 bits, and then zeros all but last 5 bits, 1f = 0001 1111
    char funct3 = (instruction >> (7+5)) & 0x00000007; //shifts down to funct3 in bottom 3 bits, and then zeros all but last 3 bits, 0x0f = 0000 0111
    char rs1 = (instruction >> (7+5+3)) & 0x0000001f; //shifts down to rs1 in bottom 5 bits, and then zeros all but last 5 bits, 0x1f = 0001 1111
    char rs2 = (instruction >> (7+5+3+5)) & 0x0000001f; //shifts down to rs2 in bottom 5 bits, and then zeros all but last 5 bits, 0x1f = 0001 1111
    char funct7 = (instruction >> (7+5+3+5+5)) & 0x0000007f; //shifts down to funct7 in bottom 7 bits, and then zeros all but last 7 bits, 0x7f = 0111 1111
    cout << (int)opcode << '\n' << (int)rd << '\n' << (int)funct3 << '\n' << (int)rs1 << '\n' << (int)rs2 << '\n' << (int)funct7 << endl;
    return 0;
}

int I(uint32_t instruction){
    char opcode = instruction & 0x0000007f; //zeros all but last 7 bit, 0x7f = 0111 1111
    char rd = (instruction >> 7) & 0x0000001f; //shifts down to rd in bottom 5 bits, and then zeros all but last 5 bits, 0x1f = 0001 1111
    char funct3 = (instruction >> (7+5)) & 0x00000007; //shifts down to funct3 in bottom 3 bits, and then zeros all but last 5 bits, 0x0f = 0000 0111
    char rs1 = (instruction >> (7+5+3)) & 0x0000001f; //shifts down to rs1 in bottom 5 bits, and then zeros all but last 5 bits, 0x1f = 0001 1111
    short imm = (instruction >> (7+5+3+5)) & 0x00000fff; //shifts down to imm in bottom 12 bits, and then zeros all but last 12 bits
    return 0;
}

int S(uint32_t instruction){
    char opcode = instruction & 0x0000007f; //zeros all but last 7 bit, 0x7f = 0111 1111
    char imm1 = (instruction >> 7) & 0x0000001f; //shifts down to imm[4:0] in bottom 5 bits, and then zeros all but last 5 bits, 0x1f = 0001 1111
    char funct3 = (instruction >> (7+5)) & 0x00000007; //shifts down to funct3 in bottom 3 bits, and then zeros all but last 5 bits, 0x0f = 0000 0111
    char rs1 = (instruction >> (7+5+3)) & 0x0000001f; //shifts down to rs1 in bottom 5 bits, and then zeros all but last 5 bits, 0x1f = 0001 1111
    char rs2 = (instruction >> (7+5+3+5)) & 0x0000001f; //shifts down to rs2 in bottom 5 bits, and then zeros all but last 5 bits, 0x1f = 0001 1111
    short imm2 = (instruction >> (7+5+3+5+5)) & 0x0000007f; //shifts down to imm[11:5] in bottom 7 bits, and then zeros all but last 7 bits
    short imm = (imm2 << 5) | imm1; 
    return 0;
}

int B(uint32_t instruction){
    char opcode = instruction & 0x0000007f; //zeros all but last 7 bit, 0x7f = 0111 1111
    char imm11 = (instruction >> 7) & 0x00000001; //shifts down to imm[11] in bottom bit, and then zeros all but last bits, 0x01 = 0000 0001
    char imm4_1 = (instruction >> 8) & 0x0000000f; //shifts down to imm[4:1] in bottom bits, and then zeros all but last 4 bits, 0x0f = 0000 1111
    char funct3 = (instruction >> (7+5)) & 0x00000007; //shifts down to funct3 in bottom 3 bits, and then zeros all but last 5 bits, 0x0f = 0000 0111
    char rs1 = (instruction >> (7+5+3)) & 0x0000001f; //shifts down to rs1 in bottom 5 bits, and then zeros all but last 5 bits, 0x1f = 0001 1111
    char rs2 = (instruction >> (7+5+3+5)) & 0x0000001f; //shifts down to rs2 in bottom 5 bits, and then zeros all but last 5 bits, 0x1f = 0001 1111
    short imm10_5 = (instruction >> (7+5+3+5+5)) & 0x0000003f; //shifts down to imm[10:5] in bottom 6 bits, and then zeros all but last 6 bits
    short imm12 = (instruction >> (7+5+3+5+5+6)) & 0x00000001; //shifts down to imm[12] in bottom bit, and then zeros all but last bit
    short imm = (imm12 << 12)| (imm11 << 11) | (imm10_5 << 5) | (imm4_1 << 1); //the pieced together imm, under the assuption that imm[0] = 0;
    return 0;
}

int U(uint32_t instruction){
    char opcode = instruction & 0x0000007f; //removes all but last 7 bit, 7f = 0111 1111
    char rd = (instruction >> 7) & 0x0000001f; //shifts down to rd in bottom 5 bits, and then zeros all but last 5 bits, 1f = 0001 1111
    uint32_t  imm31_12 = (instruction >> 12); //shifts down to imm[31:12] in bottom 20 bits
    return 0;
    }

int J(uint32_t instruction){
    char opcode = instruction & 0x0000007f; //removes all but last 7 bit, 7f = 0111 1111
    char rd = (instruction >> 7) & 0x0000001f; //shifts down to rd in bottom 5 bits, and then zeros all but last 5 bits, 1f = 0001 1111
    uint32_t imm19_12= (instruction >> (7+5)) & 0x000000ff; //shifts down to imm[19:12] in bottom 8 bits, and then zeros all but last 8 bits
    char imm11 = (instruction >> (7+5+8)) & 0x00000001;  //shifts down to imm[11] in bottom bit, and then zeros all but last bits, 0x01 = 0000 0001
    char imm10_1 = (instruction >> (7+5+8+1)) & 0x000003ff; //shifts down to imm[10_1] in bottom 10 bits, and then zeros all but last 10 bits, 0x3ff = 0011 1111 1111
    uint32_t imm20 = (instruction >> (7+5+8+1+10)) & 0x00000001;  //shifts down to imm[20] in bottom bit, and then zeros all but last bits, 0x01 = 0000 0001
    uint32_t imm = (imm20 << 20) | (imm19_12 << 12) | (imm11 << 11) | (imm10_1 << 1); //the pieced together imm, under the assuption that imm[0] = 0;
    return 0;
}

int main () {
    R(0x408505b3);// 0100 0000 1000 0101 0000 0101 1011 0011 = 0x408505b3 - corresponds to 32, 9, 10, 0, 11, 51
    return 0;
}



