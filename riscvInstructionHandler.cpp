#include <stdint.h>
#include <iostream>
#include "riscvInstructionHandler.h"

using namespace std;

uint32_t pc = 0;
uint32_t pcmax = 0;
uint32_t Reg[32]; 		// The 32 registers
uint8_t Memory[1<<20]; // the memory, an array of bytes of length 2^20


void setMemoryToZero(){
	int i = 0;
	while(i < 1<<20){
		Memory[i] = 0;
		i++;
	}
}

void initRegister(){ // Sets every value in the register to be zero
	for(int i = 0; i < 32; i++){
		Reg[i] = 0;
	}
	Reg[sp] = 1<<20-1;
}

uint32_t signExtend(uint32_t toBeExtended, uint32_t msb){ // takes an uint, and the msb (0-indexed) of that uint, then sign extends accordingly
	if(toBeExtended & (1 << msb)){
		return	toBeExtended | (0xFFFFFFF << (1 + msb));
	}else{
		return toBeExtended;
	}
}

uint32_t R(InstructionUnion instruction){ 
	uint32_t encoding =  ((uint32_t)(instruction.R_s.funct7) << 10) | ((uint32_t)(instruction.R_s.funct3) << 7) | instruction.R_s.opcode; // funct7, funct3 and opcode informs us what instruction we are dealing with
	uint64_t unsignedLongVar;  //used for unsigned multiplication
	int64_t signedLongVar; //used for signed multiplication
	uint32_t unsignedVar;  //used for unsigned multiplication
	int32_t signedVar; //used for signed multiplication
	switch(encoding){
		case 0x00033: //ADD 0 0000 0000 0011 0011 = 0x0033
			Reg[instruction.R_s.rd] = (int)Reg[instruction.R_s.rs1] + (int)Reg[instruction.R_s.rs2]; 
			break;
		case 0x08033: //SUB 0 1000 0000 0011 0011 = 0x08033
			Reg[instruction.R_s.rd] = (int)Reg[instruction.R_s.rs1] - (int)Reg[instruction.R_s.rs2];
			break;
		case 0x00b3://SLL 0 0000 0000 1011 0011 = 0x000b3
			Reg[instruction.R_s.rd] = Reg[instruction.R_s.rs1] << Reg[instruction.R_s.rs2];	
			break;
		case 0x000133://SLT 0 0000 0001 0011 0011 = 0x00133
			Reg[instruction.R_s.rd] = ((int)Reg[instruction.R_s.rs1] < (int)Reg[instruction.R_s.rs2]) ? 1 : 0;
			break;
		case 0x001b3: // SLTU 0 0000 0001 1011 0011 = 0x001b3
			Reg[instruction.R_s.rd] = (Reg[instruction.R_s.rs1] < Reg[instruction.R_s.rs2]) ? 1 : 0;
			break;
		case 0x00233: // XOR 0 0000 0010 0011 0011 = 0x0233
			Reg[instruction.R_s.rd] = Reg[instruction.R_s.rs1] ^ Reg[instruction.R_s.rs2];	
			break;
		case 0x002b3: //SRL 0 0000 0010 1011 0011 = 0x002b3, unsigned shift
			Reg[instruction.R_s.rd] = Reg[instruction.R_s.rs1] >> Reg[instruction.R_s.rs2];
			break;
		case 0x082b3: //SRA 0 1000 0010 1011 0011 = 0x082b3, signed shift
			Reg[instruction.R_s.rd] = (int)Reg[instruction.R_s.rs1] >> Reg[instruction.R_s.rs2];
			break;
		case 0x00333: //OR 0 0000 0011 0011 0011 = 0x00333
			Reg[instruction.R_s.rd] = Reg[instruction.R_s.rs1] | Reg[instruction.R_s.rs2];	
			break;
		case 0x003b3:// AND 0 0000 0011 1011 0011 = 0x03b3 
			Reg[instruction.R_s.rd] = Reg[instruction.R_s.rs1] & Reg[instruction.R_s.rs2];
			break;
		default:
			cout << hex << encoding << endl;
			cout << "Not a recognized R-type instruction" << endl;
			return 0;
    		break;
    }
    return 1;
}

uint32_t I(InstructionUnion instruction){
    uint32_t msb = 11;
	uint32_t encoding = ((uint32_t)(instruction.I_s.funct3) << 7) | instruction.I_s.opcode; // funct3 and opcode informs us what instruction we are dealing with

	switch(encoding){
		case 0x67: // JALR - 0110 0111
			Reg[instruction.I_s.rd] = pc + 4;
			pc = Reg[instruction.I_s.rs1] + signExtend((uint32_t)instruction.I_s.imm,msb) - 4;
			break;
		case 0x03: // LB - 0000 0011
			Reg[instruction.I_s.rd] = signExtend(Memory[instruction.I_s.imm + Reg[instruction.I_s.rs1]], byte-1);
			break;
		case 0x83: // LH - 1000 0011
			Reg[instruction.I_s.rd] = signExtend(((Memory[signExtend(instruction.I_s.imm,msb) + Reg[instruction.I_s.rs1] + 1] << byte) | Memory[signExtend(instruction.I_s.imm,msb) + Reg[instruction.I_s.rs1]]), 2*byte-1);
			break;
		case 0x103: //LW - 0001 0000 0011
			Reg[instruction.I_s.rd] = ((uint32_t)Memory[signExtend(instruction.I_s.imm,msb) + Reg[instruction.I_s.rs1] + 3] << 3*byte) | ((uint32_t)Memory[signExtend(instruction.I_s.imm,msb) + Reg[instruction.I_s.rs1] + 2] << 2*byte) | ((uint32_t)Memory[signExtend(instruction.I_s.imm,msb) + Reg[instruction.I_s.rs1] + 1] << byte) | (uint32_t)Memory[signExtend(instruction.I_s.imm,msb) + Reg[instruction.I_s.rs1]];
			break;
		case 0x203: //LBU - 0010 0000 0011 
			Reg[instruction.I_s.rd] = Memory[instruction.I_s.imm + Reg[instruction.I_s.rs1]];
			break;
		case 0x283: // LHU - 0010 1000 0011
			Reg[instruction.I_s.rd] = (Memory[instruction.I_s.imm + Reg[instruction.I_s.rs1] + 1] << byte) | Memory[instruction.I_s.imm + Reg[instruction.I_s.rs1]];
			break;
		case 0x13: // ADDI - 0001 0011
			Reg[instruction.I_s.rd] = Reg[instruction.I_s.rs1] +  signExtend((uint32_t)instruction.I_s.imm, msb);
			break;
		case 0x113: // SLTI - 0001 0001 0011
			Reg[instruction.I_s.rd] = ((int)Reg[instruction.I_s.rs1] < (int)signExtend(instruction.I_s.imm, msb)) ? 1 : 0;
			break;
		case 0x193: // SLTIU - 0001 1001 0011
			Reg[instruction.I_s.rd] = (Reg[instruction.I_s.rs1] < instruction.I_s.imm) ? 1 : 0;
			break;
		case 0x213: // XORI - 0010 0001 0011
			Reg[instruction.I_s.rd] = Reg[instruction.I_s.rs1] ^ instruction.I_s.imm;
			break;
		case 0x313: // ORI - 0011 0001 0011
			Reg[instruction.I_s.rd] = Reg[instruction.I_s.rs1] | instruction.I_s.imm;
			break;
		case 0x393: // ANDI - 0011 1001 0011
			Reg[instruction.I_s.rd] = Reg[instruction.I_s.rs1] & instruction.I_s.imm;
			break;
		case 0x093: // SLLI 0000 1001 0011 = 0x00093 //ASK TA
			Reg[instruction.II_s.rd] = Reg[instruction.II_s.rs1] << instruction.II_s.shamt;
			break;
		case 0x293: // SRLI 0010 1001 0011 = 0x00293 	
			if (instruction.II_s.funct7 == 0){
				Reg[instruction.II_s.rd] = Reg[instruction.II_s.rs1] >> instruction.II_s.shamt;
			} else{ // SRAI 0 1000
				Reg[instruction.II_s.rd] = ((int)Reg[instruction.II_s.rs1] >> instruction.II_s.shamt); //arithmatic right shifting is a thing in C++, but only for signed integers
			}
			break;
		//case 0x08293://SRAI 0 1000 0010 1001 0011 = 0x08293 //just shift w. shamt in rs2's place
		//	Reg[instruction.R_s.rd] = signExtend((Reg[instruction.R_s.rs1] >> Reg[instruction.R_s.rs2]), 31-Reg[instruction.R_s.rs2]);
		//	break; 
		default:
			cout << "Not a recognized I-type instruction" << endl;
	    	return 0;
    		break;
    }
    return 1;
}


uint32_t S(InstructionUnion instruction){ 
	int imm = signExtend(((instruction.S_s.imm11_5) << 5) | instruction.S_s.imm4_0,11); 


	switch(instruction.S_s.funct3){
    	case 0x0:	// SB - 000
    		Memory[Reg[instruction.S_s.rs1] + imm 	 ] =  Reg[instruction.S_s.rs2] 	& 0xFF; // Only stores the first byte
    		break;
    	case 0x1:  	// SH - 001
    		Memory[Reg[instruction.S_s.rs1] + imm 	 ] =  Reg[instruction.S_s.rs2] 	& 0xFF; // First byte
    		Memory[Reg[instruction.S_s.rs1] + imm + 1] = (Reg[instruction.S_s.rs2]  & 0xFF00) >> byte; // Second byte     	 
    		break;
    	case 0x2:	// SW - 010
	   		Memory[Reg[instruction.S_s.rs1] + imm 	 ] =   Reg[instruction.S_s.rs2] & 0xFF; 				  // First byte
    		Memory[Reg[instruction.S_s.rs1] + imm + 1] = ((Reg[instruction.S_s.rs2] & 0xFF00) 	  >>   byte); // Second byte
    		Memory[Reg[instruction.S_s.rs1] + imm + 2] = ((Reg[instruction.S_s.rs2] & 0xFF0000)   >> 2*byte); // Third byte
    		Memory[Reg[instruction.S_s.rs1] + imm + 3] = ((Reg[instruction.S_s.rs2] & 0xFF000000) >> 3*byte); // What do you think byte
    		break;
    	default :
			cout << "Not a recognized S-type instruction" << endl;
    		return 0;
    		break;
    }
    return 1;
}

uint32_t B(InstructionUnion instruction){
	uint32_t imm = signExtend(((uint32_t)(instruction.B_s.imm12) << 12)| ((uint32_t)(instruction.B_s.imm11) << 11) | ((uint32_t)(instruction.B_s.imm10_5) << 5) | ((uint32_t)(instruction.B_s.imm4_1) << 1),11); //the pieced together instruction.I_s.imm, under the assuption that instruction.I_s.imm[0] = 0;
    


//	cout << "immediate = " << hex << imm << endl;

	switch(instruction.B_s.funct3){
		case 0x0: // beq - 000
			if((int)Reg[instruction.B_s.rs1] == (int)Reg[instruction.B_s.rs2]){
				pc = pc + (int)imm - 4; // The -4 is to compensate for the fact that we always add 4 at the end of the loop
			}
			break;
		case 0x1: // bne - 001
			if((int)Reg[instruction.B_s.rs1] != (int)Reg[instruction.B_s.rs2]){
				pc = pc + (int)imm - 4;
			}
			break;
		case 0x4: // blt - 100
			if((int)Reg[instruction.B_s.rs1] < (int)Reg[instruction.B_s.rs2]){
				pc = pc + (int)imm - 4;
			}
			break;
		case 0x5: // bge - 101 
			if((int)Reg[instruction.B_s.rs1] >= (int)Reg[instruction.B_s.rs2]){
				pc = pc + (int)imm - 4;
			}
			break;
		case 0x6: // bltu - 110
			if(Reg[instruction.B_s.rs1] < Reg[instruction.B_s.rs2]){
				pc = pc + (int)imm - 4;
			}
			break;
		case 0x7: // bgeu - 111
			if(Reg[instruction.B_s.rs1] >= Reg[instruction.B_s.rs2]){
				pc = pc + (int)imm - 4;
			}
			break;	
		default:
			cout << "Not a recognized B-type instruction" << endl;
    		return 0;
    		break;
    }
    return 1;
}

uint32_t U(InstructionUnion instruction){
    uint32_t imm = (uint32_t)(instruction.U_s.imm31_12) << 12;

    switch(instruction.U_s.opcode){
    	case 0x17: // AUIPC - 0010111
    		Reg[instruction.U_s.rd] = pc + imm;
    		break;
    	case 0x37: // LUI - 0110111
    		Reg[instruction.U_s.rd] = imm;
    		break;
    	default:
			cout << "Not a recognized U-type instruction" << endl;
    		return 0;
    		break;
    }
    return 1;
}

uint32_t J(InstructionUnion instruction){
    uint32_t imm = signExtend(((uint32_t)(instruction.J_s.imm20) << 20) | ((uint32_t)(instruction.J_s.imm19_12) << 12) | ((uint32_t)(instruction.J_s.imm11) << 11) | ((uint32_t)(instruction.J_s.imm10_1) << 1),19);	 
    switch(instruction.J_s.opcode){
    	case 0x6F: // JAL - 1101111
    		Reg[instruction.J_s.rd] = pc + 4;
    		pc = pc + (int)imm - 4; // -4 to compensate for +4 in the while loop
    		break;
    	default:
			cout << "Not a recognized J-type instruction" << endl;
    		return 0;
    		break;
    }
    return 1;
}


uint32_t X(){
	switch(Reg[a0]){
		case 1:
			cout << hex << Reg[a1];
			break;
		case 10:
			cout << "Program finished succesfully" << endl;
			return 0;
			break;
		case 11:
			cout << (char)Reg[a1];
			break;
		case 17:
			cout << hex << Reg[a1] << endl;
			cout << "closing" << endl;
			return 0;
			break;
		default:
			cout << "Not a recognized ecall, content of a0 is: " << Reg[a0] << endl;
    		break;
	}
	return 1;
}

char whatKindOfInstruction(InstructionUnion instruction){ // Looks at the opcode (and in one case funct3) and figures out which type of
														  // Instruction we are dealing with
	switch(instruction.B_s.opcode){ //001 0011 = 0x313
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
//				cout << 'I' << " " << instruction.R_s.funct3 << '\n';					  //Please note that it could have been instruction.I_s.funct3
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
