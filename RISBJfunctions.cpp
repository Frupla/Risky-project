#include <iostream>
#include <stdint.h>
using namespace std;

union InstructionUnion  {
    uint32_t instruction;
    struct {
        uint32_t opcode: 7;
        uint32_t rd: 5;
        uint32_t funct3: 3;
        uint32_t instruction.I_s.rs1: 5;
        uint32_t rs2: 5;
        uint32_t funct7: 7;
    } R_s;
    struct {
        uint32_t opcode: 7;
        uint32_t rd: 5;
        uint32_t funct3: 3;
        uint32_t instruction.I_s.rs1: 5;
        uint32_t instruction.I_s.imm: 12;
    } I_s;
    struct {
        uint32_t opcode: 7;
        uint32_t instruction.I_s.imm4_0: 5;
        uint32_t funct3: 3;
        uint32_t instruction.I_s.rs1: 5;
        uint32_t rs2: 5;
        uint32_t instruction.I_s.imm11_5: 7;
    } S_s;
    struct {
        uint32_t opcode: 7;
        uint32_t instruction.I_s.imm11: 1;
        uint32_t instruction.I_s.imm4_1: 4;
        uint32_t funct3: 3;
        uint32_t instruction.I_s.rs1: 5;
        uint32_t rs2: 5;
        uint32_t instruction.I_s.imm10_5: 6;
        uint32_t instruction.I_s.imm12: 1;
    } B_s;
    struct 
    {
        uint32_t opcode: 7;
        uint32_t rd: 5;
        uint32_t instruction.I_s.imm31_12: 20;
    } U_s;
    struct
    {
        uint32_t opcode: 7;
        uint32_t rd: 5;
        uint32_t instruction.I_s.imm19_12: 8;
        uint32_t instruction.I_s.imm11: 1;
        uint32_t instruction.I_s.imm10_1: 10;
        uint32_t instruction.I_s.imm20: 1; 
    }J_s;
};

int R(uint32_t instruction){
    //struct R_s r = (struct R_s)instruction;
    /*char opcode = instruction & 0x0000007f; //removes all but last 7 bit, 7f = 0111 1111
    char rd = (instruction >> 7) & 0x0000001f; //shifts down to rd in bottom 5 bits, and then zeros all but last 5 bits, 1f = 0001 1111
    char funct3 = (instruction >> (7+5)) & 0x00000007; //shifts down to funct3 in bottom 3 bits, and then zeros all but last 3 bits, 0x0f = 0000 0111
    char instruction.I_s.rs1 = (instruction >> (7+5+3)) & 0x0000001f; //shifts down to instruction.I_s.rs1 in bottom 5 bits, and then zeros all but last 5 bits, 0x1f = 0001 1111
    char rs2 = (instruction >> (7+5+3+5)) & 0x0000001f; //shifts down to rs2 in bottom 5 bits, and then zeros all but last 5 bits, 0x1f = 0001 1111
    char funct7 = (instruction >> (7+5+3+5+5)) & 0x0000007f; //shifts down to funct7 in bottom 7 bits, and then zeros all but last 7 bits, 0x7f = 0111 1111
    */
    InstructionUnion  r;
    r.instruction = instruction;
    //cout << (int)(r.R_s.opcode) << '\n' << (int)(r.R_s.rd) << '\n' << (int)(r.R_s.funct3) << '\n' << (int)(r.R_s.instruction.I_s.rs1) << '\n' << (int)(r.R_s.rs2) << '\n' << (int)(r.R_s.funct7) << endl;
    return 0;
}

int I(uint32_t instruction){
    /*
    char opcode = instruction & 0x0000007f; //zeros all but last 7 bit, 0x7f = 0111 1111
    char rd = (instruction >> 7) & 0x0000001f; //shifts down to rd in bottom 5 bits, and then zeros all but last 5 bits, 0x1f = 0001 1111
    char funct3 = (instruction >> (7+5)) & 0x00000007; //shifts down to funct3 in bottom 3 bits, and then zeros all but last 5 bits, 0x0f = 0000 0111
    char instruction.I_s.rs1 = (instruction >> (7+5+3)) & 0x0000001f; //shifts down to instruction.I_s.rs1 in bottom 5 bits, and then zeros all but last 5 bits, 0x1f = 0001 1111
    uint32_t instruction.I_s.imm = (instruction >> (7+5+3+5)) & 0x00000fff; //shifts down to instruction.I_s.imm in bottom 12 bits, and then zeros all but last 12 bits
    */
    InstructionUnion  i;
    i.instruction = instruction;
    //cout << (int)(i.I_s.opcode) << '\n' << (int)(i.I_s.rd) << '\n' << (int)(i.I_s.funct3) << '\n' << (int)(i.I_s.instruction.I_s.rs1) << '\n' << (int)(i.I_s.instruction.I_s.imm) << endl;
    return 0;
}

int S(uint32_t instruction){
    /*
    char opcode = instruction & 0x0000007f; //zeros all but last 7 bit, 0x7f = 0111 1111
    char instruction.I_s.imm1 = (instruction >> 7) & 0x0000001f; //shifts down to instruction.I_s.imm[4:0] in bottom 5 bits, and then zeros all but last 5 bits, 0x1f = 0001 1111
    char funct3 = (instruction >> (7+5)) & 0x00000007; //shifts down to funct3 in bottom 3 bits, and then zeros all but last 5 bits, 0x0f = 0000 0111
    char instruction.I_s.rs1 = (instruction >> (7+5+3)) & 0x0000001f; //shifts down to instruction.I_s.rs1 in bottom 5 bits, and then zeros all but last 5 bits, 0x1f = 0001 1111
    char rs2 = (instruction >> (7+5+3+5)) & 0x0000001f; //shifts down to rs2 in bottom 5 bits, and then zeros all but last 5 bits, 0x1f = 0001 1111
    uint32_t instruction.I_s.imm2 = (instruction >> (7+5+3+5+5)) & 0x0000007f; //shifts down to instruction.I_s.imm[11:5] in bottom 7 bits, and then zeros all but last 7 bits
    */
    InstructionUnion  s;
    s.instruction = instruction;
    uint32_t instruction.I_s.imm = ((uint32_t)(s.S_s.instruction.I_s.imm11_5) << 5) | s.S_s.instruction.I_s.imm4_0; 
    //cout << (int)opcode << '\n' <<  (int)instruction.I_s.imm1 << '\n' <<  (int)funct3 << '\n' <<  (int)instruction.I_s.rs1 << '\n' <<  (int)rs2 << '\n' <<  (int)instruction.I_s.imm2 << '\n'; 
    return 0;
}

int B(uint32_t instruction){
    /*char opcode = instruction & 0x0000007f; //zeros all but last 7 bit, 0x7f = 0111 1111
    char instruction.I_s.imm11 = (instruction >> 7) & 0x00000001; //shifts down to instruction.I_s.imm[11] in bottom bit, and then zeros all but last bits, 0x01 = 0000 0001
    char instruction.I_s.imm4_1 = (instruction >> 8) & 0x0000000f; //shifts down to instruction.I_s.imm[4:1] in bottom bits, and then zeros all but last 4 bits, 0x0f = 0000 1111
    char funct3 = (instruction >> (7+5)) & 0x00000007; //shifts down to funct3 in bottom 3 bits, and then zeros all but last 5 bits, 0x0f = 0000 0111
    char instruction.I_s.rs1 = (instruction >> (7+5+3)) & 0x0000001f; //shifts down to instruction.I_s.rs1 in bottom 5 bits, and then zeros all but last 5 bits, 0x1f = 0001 1111
    char rs2 = (instruction >> (7+5+3+5)) & 0x0000001f; //shifts down to rs2 in bottom 5 bits, and then zeros all but last 5 bits, 0x1f = 0001 1111
    uint32_t instruction.I_s.imm10_5 = (instruction >> (7+5+3+5+5)) & 0x0000003f; //shifts down to instruction.I_s.imm[10:5] in bottom 6 bits, and then zeros all but last 6 bits
    uint32_t instruction.I_s.imm12 = (instruction >> (7+5+3+5+5+6)) & 0x00000001; //shifts down to instruction.I_s.imm[12] in bottom bit, and then zeros all but last bit
    */
    InstructionUnion  b;
    b.instruction = instruction;
    uint32_t instruction.I_s.imm = ((uint32_t)(b.B_s.instruction.I_s.imm12) << 12)| ((uint32_t)(b.B_s.instruction.I_s.imm11) << 11) | ((uint32_t)(b.B_s.instruction.I_s.imm10_5) << 5) | ((uint32_t)(b.B_s.instruction.I_s.imm4_1) << 1); //the pieced together instruction.I_s.imm, under the assuption that instruction.I_s.imm[0] = 0;
    cout << (int)instruction.I_s.imm << '\n' << (int)(b.B_s.rs2) << '\n' << (int)(b.B_s.instruction.I_s.rs1)  << '\n' << (int)(b.B_s.funct3)  << '\n' << (int)(b.B_s.opcode ) << '\n';
    return 0;
}

int U(uint32_t instruction){
    /*
    char opcode = instruction & 0x0000007f; //removes all but last 7 bit, 7f = 0111 1111
    char rd = (instruction >> 7) & 0x0000001f; //shifts down to rd in bottom 5 bits, and then zeros all but last 5 bits, 1f = 0001 1111
    uint32_t instruction.I_s.imm = instruction & 0xfffff000; //sets instruction.I_s.imm[31:12] in the top 20 bits and zeros the remaning 12
    */
    InstructionUnion  u;
    u.instruction = instruction;
    uint32_t instruction.I_s.imm = (uint32_t)(u.U_s.instruction.I_s.imm) << 12;
    //cout <<(int)opcode  << '\n' << (int)rd << '\n' << (int)instruction.I_s.imm  << '\n';
    return 0;
    }

int J(uint32_t instruction){
    /*
    char opcode = instruction & 0x0000007f; //removes all but last 7 bit, 7f = 0111 1111
    char rd = (instruction >> 7) & 0x0000001f; //shifts down to rd in bottom 5 bits, and then zeros all but last 5 bits, 1f = 0001 1111
    uint32_t instruction.I_s.imm19_12= (instruction >> (7+5)) & 0x000000ff; //shifts down to instruction.I_s.imm[19:12] in bottom 8 bits, and then zeros all but last 8 bits
    char instruction.I_s.imm11 = (instruction >> (7+5+8)) & 0x00000001;  //shifts down to instruction.I_s.imm[11] in bottom bit, and then zeros all but last bits, 0x01 = 0000 0001
    uint32_t instruction.I_s.imm10_1 = (instruction >> (7+5+8+1)) & 0x000003ff; //shifts down to instruction.I_s.imm[10_1] in bottom 10 bits, and then zeros all but last 10 bits, 0x3ff = 0011 1111 1111
    uint32_t instruction.I_s.imm20 = (instruction >> (7+5+8+1+10)) & 0x00000001;  //shifts down to instruction.I_s.imm[20] in bottom bit, and then zeros all but last bits, 0x01 = 0000 0001
    */

    InstructionUnion  j;
    j.instruction = instruction;
    uint32_t instruction.I_s.imm = ((uint32_t)(j.J_s.instruction.I_s.imm20) << 20) | ((uint32_t)(j.J_s.instruction.I_s.imm19_12) << 12) | ((uint32_t)(j.J_s.instruction.I_s.imm11) << 11) | ((uint32_t)(j.J_s.instruction.I_s.imm10_1) << 1); //the pieced together instruction.I_s.imm, under the assuption that instruction.I_s.imm[0] = 0;
    //cout << (int)opcode << '\n' << (int)rd << '\n' << (int)instruction.I_s.imm << '\n'; //<< (int)instruction.I_s.imm10_1 << '\n' << (int)instruction.I_s.imm20 << '\n';
    return 0;
}

int main () {
    //R(0x408505b3);// 0100 0000 1000 0101 0000 0101 1011 0011 =  0x408505b3 - corresponds to 32, 8, 10, 0, 11, 51
    //I(0x893491c9);// 1000 1001 0011 0100 1001 0001 1100 1001 = 0x893491c9, instruction.I_s.imm 1000 1001 0011 = 2195,instruction.I_s.rs1 0100 1 = 9, funct3 001 =1,rd 0001 1=3, opcode 100 1001=73
    //S(0x408505b3);// opcode 51, instruction.I_s.imm1 11, funct3 0, instruction.I_s.rs1 = 10, rs2 = 8, instruction.I_s.imm2 = 32
    B(0x408505b3);// instruction.I_s.imm12:0 - instruction.I_s.imm10_5: 100 000 rs2: 0 1000=8 - instruction.I_s.rs1: 0 1010=10 - funct3: 000=0 -instruction.I_s.imm4_1: 0101=5 instruction.I_s.imm11: 1 - opcode: 011 0011=51
                   // instruction.I_s.imm: 0 1100 0000 1010 = 3082 rs2: 8 - instruction.I_s.rs1: 10 - funct3: 0 opcode: 51
    //U(0x408505b3); // opcode: 51, rd: 11, instruction.I_s.imm: 264272
    //J(0x408505b3); //opcode: 51, rd: 11, 
    // instruction.I_s.imm20: 0- instruction.I_s.imm10_1: 100 0000 100 - instruction.I_s.imm11: 0 -  instruction.I_s.imm19_12: 0101 0000
    // instruction.I_s.imm = 0 0101 0000 0100 0000 1000 = 328712
    return 0;
}

