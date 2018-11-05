#include <iostream>
#include <stdint.h>


using namespace std;

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

uint32_t Reg[32];
char Memory[2*2*2*2*2*2*2*2*2*2*2*2*2*2*2*2*2];

uint32_t pc = 0;


uint32_t signExtend(uint32_t toBeExtended, uint32_t msb){
	if(toBeExtended & (1 << msb)){
		return	toBeExtended | (0xFFFFFFF << (1 + msb));
	}else{
		return toBeExtended;
	}
}


uint32_t R(InstructionUnion instruction){ //not done yet, I got distracted -ID
	uint32_t encoding =  ((uint32_t)(instruction.R_s.funct7) << 10) | ((uint32_t)(instruction.R_s.funct3) << 7) | instruction.R_s.opcode; // funct7, funct3 and opcode informs us what instruction we are dealing with
	switch(encoding){
		case 0x00093://SLLI 0 0000 0000 1001 0011 = 0x00093
			break;
		case 0x00293://SRLI 0 0000 0010 1001 0011 = 0x00293
			break;
		case 0x08293://SRAI 0 1000 0010 1001 0011 = 0x08293
			break;
		case 0x0033://ADD 0 0000 0000 0011 0011 = 0x0033
			break;
		case 0x08033://SUB 0 1000 0000 0011 0011 = 0x08033
			break;
		//SLL
		default:
			cout << "Opcode " << instruction.R_s.opcode << " not yet implemented";
			break;
	}
	return 0;
}

uint32_t I(InstructionUnion instruction){
    uint32_t msb = 11;
	uint32_t encoding = ((uint32_t)(instruction.I_s.funct3) << 7) | instruction.I_s.opcode; // funct3 and opcode informs us what instruction we are dealing with

	switch(encoding){
		case 0x67: // JALR
			Reg[instruction.I_s.rd] = ++pc;
			pc = Reg[instruction.I_s.rs1] + instruction.I_s.imm;
			break;
		case 0x03: // LB
			Reg[instruction.I_s.rd] = Memory[instruction.I_s.imm + instruction.I_s.rs1] | signExtend(Memory[instruction.I_s.imm + instruction.I_s.rs1], byte-1);
			break;
		case 0x83: // LH
			Reg[instruction.I_s.rd] = (Memory[instruction.I_s.imm + instruction.I_s.rs1] << byte) | Memory[instruction.I_s.imm + instruction.I_s.rs1 + 1] | signExtend(((Memory[instruction.I_s.imm + instruction.I_s.rs1] << byte) | Memory[instruction.I_s.imm + instruction.I_s.rs1 + 1]), 2*byte-1);
			break;
		case 0x103: //LW
			Reg[instruction.I_s.rd] = (Memory[instruction.I_s.imm + instruction.I_s.rs1] << 3*byte) | (Memory[instruction.I_s.imm + instruction.I_s.rs1 + 1] << 2*byte | Memory[instruction.I_s.imm + instruction.I_s.rs1 + 2] << byte) | Memory[instruction.I_s.imm + instruction.I_s.rs1 + 3];
			break;
		case 0x203: //LBU
			Reg[instruction.I_s.rd] = Memory[instruction.I_s.imm + instruction.I_s.rs1];
			break;
		case 0x283: // LHU
			Reg[instruction.I_s.rd] = (Memory[instruction.I_s.imm + instruction.I_s.rs1] << byte) | Memory[instruction.I_s.imm + instruction.I_s.rs1 + 1];
			break;
		case 0x13: // ADDI
			Reg[instruction.I_s.rd] = Reg[instruction.I_s.rs1] + instruction.I_s.imm;
			break;
		case 0x113: // SLTI
			Reg[instruction.I_s.rd] = ((int)Reg[instruction.I_s.rs1] < (int)signExtend(instruction.I_s.imm, msb)) ? 1 : 0;
			break;
		case 0x193: // SLTIU	
			Reg[instruction.I_s.rd] = (Reg[instruction.I_s.rs1] < instruction.I_s.imm) ? 1 : 0;
			break;
		case 0x213: // XORI
			Reg[instruction.I_s.rd] = Reg[instruction.I_s.rs1] ^ instruction.I_s.imm;
			break;
		case 0x313: // ORI
			Reg[instruction.I_s.rd] = Reg[instruction.I_s.rs1] | instruction.I_s.imm;
			break;
		case 0x393: // ANDI
			Reg[instruction.I_s.rd] = Reg[instruction.I_s.rs1] & instruction.I_s.imm;
			break;
	}
	return 0;
}



char whatKindOfInstruction(InstructionUnion instruction){
	switch(instruction.B_s.opcode){
		case 0x23 : // S - type
			return 'S';
			break;
		case 0x63 : // B - type
			return 'B';
			break;
		case 0x6F : // J - type
			return 'J';
			break;
		case 0x37  : // U - type
			return 'U';
			break;
		case 0x17  : // U - type
			return 'U';
			break; 
		case 0x67 : // I - type
			return 'I';
			break;
		case 0x03 : // I - type
			return 'I';
			break;
		case 0x13 : // I OR R - type
			if((instruction.R_s.funct3 != 0x1000) && (instruction.R_s.funct3 != 0x5000)){ // Handling the #NotAllinstruction.I_s.immediates problem
																						  //Please note that it could have been instruction.I_s.funct3
				return 'I';
			}else{
				return 'R';
			}
			break;
		case 0x33 : // R - type
			return 'R';
			break;
		case 0x73: // Ecall
			return 'X';
			break;
		default:
			return '?';
			break;
	}

}


int main(){

	uint32_t prog[100];
	int pcmax = 100; // This should be the lenght of the prog array
	InstructionUnion instruction;
	//instruction.instruction = 0x408505b3;
	char instructionType;
	uint32_t branchInstruction = 0;

	while(pc < pcmax){
		cin >> instruction.instruction;
		//instruction.instruction = prog[pc];

		instructionType = whatKindOfInstruction(instruction);


		switch(instructionType){
			case 'R':
//				R(instruction);
				cout << instructionType << endl;
				break;
			case 'I':
//				I(instruction);
				cout << instructionType << endl;
				break;
			case 'S':
//				S(instruction);
				cout << instructionType << endl;
				break;
			case 'U':
//				U(instruction);
				cout << instructionType << endl;
				break;
			case 'B':
//				B(instruction);
				cout << instructionType << endl;
				break;
			case 'J':
//				J(instruction);
				cout << instructionType << endl;
				break;
			case 'X':
				pc = pcmax;
				cout << "closing" << endl;
				break;
			default:
				cout << "Invalid input" << endl;
				break;		
		}

		pc++;
	}



	return 0;
}