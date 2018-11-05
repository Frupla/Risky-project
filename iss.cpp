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

uint32_t I(uint32_t instruction){
	uint32_t msb = 11;
	char opcode = instruction & 0x0000007f; //zeros all but last 7 bit, 0x7f = 0111 1111
    char rd = (instruction >> 7) & 0x0000001f; //shifts down to rd in bottom 5 bits, and then zeros all but last 5 bits, 0x1f = 0001 1111
    char funct3 = (instruction >> (7+5)) & 0x00000007; //shifts down to funct3 in bottom 3 bits, and then zeros all but last 5 bits, 0x0f = 0000 0111
    char rs1 = (instruction >> (7+5+3)) & 0x0000001f; //shifts down to rs1 in bottom 5 bits, and then zeros all but last 5 bits, 0x1f = 0001 1111
    short imm = (instruction >> (7+5+3+5)) & 0x00000fff; //shifts down to imm in bottom 12 bits, and then zeros all but last 12 bits

	uint32_t encoding = (funct3 << 7) | opcode; // funct3 and opcode informs us what instruction we are dealing with

	switch(encoding){
		case 0x67: // JALR
			Reg[rd] = ++pc;
			pc = Reg[rs1] + imm;
			break;
		case 0x03: // LB
			Reg[rd] = Memory[imm + rs1] | signExtend(Memory[imm + rs1], byte-1);
			break;
		case 0x83: // LH
			Reg[rd] = (Memory[imm + rs1] << byte) | Memory[imm + rs1 + 1] | signExtend(((Memory[imm + rs1] << byte) | Memory[imm + rs1 + 1]), 2*byte-1);
			break;
		case 0x103: //LW
			Reg[rd] = (Memory[imm + rs1] << 3*byte) | (Memory[imm + rs1 + 1] << 2*byte | Memory[imm + rs1 + 2] << byte) | Memory[imm + rs1 + 3];
			break;
		case 0x203: //LBU
			Reg[rd] = Memory[imm + rs1];
			break;
		case 0x283: // LHU
			Reg[rd] = (Memory[imm + rs1] << byte) | Memory[imm + rs1 + 1];
			break;
		case 0x13: // ADDI
			Reg[rd] = Reg[rs1] + imm;
			break;
		case 0x113: // SLTI
			Reg[rd] = ((int)Reg[rs1] < (int)signExtend(imm, msb)) ? 1 : 0;
			break;
		case 0x193: // SLTIU	
			Reg[rd] = (Reg[rs1] < imm) ? 1 : 0;
			break;
		case 0x213: // XORI
			Reg[rd] = Reg[rs1] ^ imm;
			break;
		case 0x313: // ORI
			Reg[rd] = Reg[rs1] | imm;
			break;
		case 0x393: // ANDI
			Reg[rd] = Reg[rs1] & imm;
			break;
	}

}



char whatKindOfInstruction(uint32_t instruction){
	uint32_t bitString = instruction & 0x7F;
	uint32_t funct3;
	switch(bitString){
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
			funct3 = (instruction & 0x7000);
			if((funct3 != 0x1000) && (funct3 != 0x5000)){ // Handling the #NotAllImmediates problem
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
	int pcmax = 100; // This should be the length of the prog array
	
	uint32_t instruction;
	char instructionType;
	uint32_t branchInstruction = 0;

	while(pc < pcmax){
		cin >> instruction;
		//instruction = prog[pc];

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