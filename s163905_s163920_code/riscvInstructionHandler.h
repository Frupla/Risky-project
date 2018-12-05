#ifndef RISCVINSTRUCTIONHANDLER
#define RISCVINSTRUCTIONHANDLER

// Shady bitfield implementation
union InstructionUnion {
    uint32_t instruction;
    struct {
        uint32_t opcode: 7;
        uint32_t rd: 5;
        uint32_t funct3: 3;
        uint32_t rs1: 5;
        uint32_t rs2: 5;
        uint32_t funct7: 7;
    } R_s;
    struct {
        uint32_t opcode: 7;
        uint32_t rd: 5;
        uint32_t funct3: 3;
        uint32_t rs1: 5;
        uint32_t imm: 12;
    } I_s;
    struct {
        uint32_t opcode: 7;
        uint32_t rd: 5;
        uint32_t funct3: 3;
        uint32_t rs1: 5;
        uint32_t shamt: 5;
        uint32_t funct7: 7;
    } II_s;
    struct {
        uint32_t opcode: 7;
        uint32_t imm4_0: 5;
        uint32_t funct3: 3;
        uint32_t rs1: 5;
        uint32_t rs2: 5;
        uint32_t imm11_5: 7;
    } S_s;
    struct {
        uint32_t opcode: 7;
        uint32_t imm11: 1;
        uint32_t imm4_1: 4;
        uint32_t funct3: 3;
        uint32_t rs1: 5;
        uint32_t rs2: 5;
        uint32_t imm10_5: 6;
        uint32_t imm12: 1;
    } B_s;
    struct 
    {
        uint32_t opcode: 7;
        uint32_t rd: 5;
        uint32_t imm31_12: 20;
    } U_s;
    struct
    {
        uint32_t opcode: 7;
        uint32_t rd: 5;
        uint32_t imm19_12: 8;
        uint32_t imm11: 1;
        uint32_t imm10_1: 10;
        uint32_t imm20: 1; 
    }J_s;
};
#define x0  0
#define x1  1
#define x2  2
#define x3  3
#define x4  4
#define x5  5
#define x6  6
#define x7  7
#define x8  8
#define x9  9
#define x10 10
#define x11 11
#define x12 12
#define x13 13
#define x14 14
#define x15 15
#define x16 16
#define x17 17
#define x18 18
#define x19 19
#define x20 20
#define x21 21
#define x22 22
#define x23 23
#define x24 24
#define x25 25
#define x26 26
#define x27 27
#define x28 28
#define x29 29
#define x30 30
#define x31 31

#define zero  0
#define ra	  1
#define sp	  2
#define gp	  3
#define tp 	  4
#define t0    5
#define t1	  6
#define t2	  7
#define fp	  8
#define s0 	  8
#define s1    9
#define a0   10
#define a1   11
#define a2   12
#define a3   13
#define a4   14
#define a5   15
#define a6   16
#define a7   17
#define s2   18
#define s3   19
#define s4   20
#define s5   21
#define s6   22
#define s7   23
#define s8   24
#define s9   25
#define s10  26
#define s11  27
#define t3   28
#define t4   29
#define t5   30
#define t6   31
#define byte 8

// Global variables (It is nice to have access to these across all functions)
extern uint32_t Reg[32]; 		// The 32 registers
extern uint8_t Memory[1<<20]; // the memory, an array of bytes of length 2^20
extern uint32_t pc; 		// the program counter
extern uint32_t pcmax;

uint32_t signExtend(uint32_t toBeExtended, uint32_t msb); // takes an uint, and the msb (0-indexed) of that uint, then sign extends accordingly
uint32_t R(InstructionUnion instruction);
uint32_t I(InstructionUnion instruction);
uint32_t S(InstructionUnion instruction);
uint32_t B(InstructionUnion instruction);
uint32_t U(InstructionUnion instruction);
uint32_t J(InstructionUnion instruction);
uint32_t X();
char whatKindOfInstruction(InstructionUnion instruction);
void setMemoryToZero();
void initRegister();

#endif