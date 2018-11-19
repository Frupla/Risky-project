#include <iostream>
#include <stdint.h>
#include <fstream>
#include <string.h>

//TODO: ask TA's about how to implement SLLI, SRLI and SRAI
//      ask TA's if we can always assume programs end on ecall, or on newline or what
//					essentially what are the end conditions for a program
using namespace std;


// So we can just call registers their names, though, we might never have to use this...

// Oh well, we can alwas delete this
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


// Global variables (It is nice to have access to these across all functions)
static int lengthOfMemory = 1<<10;
uint32_t Reg[32]; 		// The 32 registers
uint32_t pc = 0; 		// the program counter
uint8_t Memory[1<<10]; // the memory, an array of bytes of length 2^10
uint32_t pcmax = 0;

void setMemoryToZero(){
	for(int i = 0; i <= lengthOfMemory; i++){
		Memory[i] = 0;
	}
}

void printMemory(){
	for (int i = 0; i <= lengthOfMemory; i++){
		if(Memory[i] != 0){
			cout << "Memory[" << dec << i << "] = " << hex << (int)Memory[i] << endl;
		}
	}
}

void printRegister(){
	for(int i = 0; i < 32; i++){
		cout << "x" << dec << i << " = " << dec << (int)Reg[i] << endl;
	}
}

void initRegister(){ // Sets every value in the register to be zero
	for(int i = 0; i < 32; i++){
		Reg[i] = 0;
	}
	Reg[sp] = lengthOfMemory - 1; //Initializing the stack pointer to the last memory value.
}

uint32_t signExtend(uint32_t toBeExtended, uint32_t msb){ // takes an uint, and the msb (0-indexed) of that uint, then sign extends accordingly
	if(toBeExtended & (1 << msb)){
		return	toBeExtended | (0xFFFFFFF << (1 + msb));
	}else{
		return toBeExtended;
	}
}

int shittyInput(){
	uint16_t input;
	int i = 0;
	while(input != 0x61){ // - Break when Ecall
		cin >> hex >> input;
		Memory[i] = (uint8_t)(input>>byte);
		i++;
		Memory[i] = (uint8_t)(input);
		i++;
	}
	return i;
}

void printProgram(int n){

	for(int i = 0; i <= n; i = i + 4){
		cout << dec << "Memory[" << i << ";" << i+3 << "] = ";
		cout << hex << (uint32_t)Memory[i+3] << " ";
		cout << hex << (uint32_t)Memory[i+2] << " ";
		cout << hex << (uint32_t)Memory[i+1] << " ";
		cout << hex << (uint32_t)Memory[i] << endl;
	}	
}

bool readFileIntoMemory(){
	string filename;
	cin >> filename;
	int i = 0;
	streampos fileSize;

	char * temporaryMemory;
	temporaryMemory = new char [2];
	ifstream file (filename, ios::in|ios::binary|ios::ate);//open file and set pointer at end of file
	if (file.is_open())
	{
		cout << "file is open" << endl;
		fileSize = file.tellg(); //use pointer to get file size
		cout << fileSize << endl;
		file.seekg (0, ios::beg); //set pointer to beginning of file 
		while(file.tellg() <= fileSize-(streampos)2){
			file.read (temporaryMemory, 2*sizeof(char)); //should also update file pointer
			Memory[i] = (uint8_t)temporaryMemory[0];
			i++;
			Memory[i] = (uint8_t)temporaryMemory[1];
			i++;
			//cout << i << endl;
		}
		cout << "done reading in data" << endl;
		delete[] temporaryMemory;
	}else {
		cout << "Unable to open file" << endl;
		return 1;
	}
	pcmax = i;	
	return 0;
}

uint32_t debug(){ // This uses a weird syntax, but tbh, it is far better than writing in commands as long decimals
	// The input is four integers in array A, the syntax is as follow:
	// A[0] determines what kind of instruction we are dealing with, 0 is addi, 1 is lw, 2 is sw and 3 is a generic R instruction (right now I'm using R to print the memory)
	// A[1] is the register where the result of the instruction should be saved
	// A[2] is the second register, often the value here is often manipulated in some fashion
	// A[3] is the immediate, often added to the second register
	InstructionUnion instruction;
	int input;
	int A[4];
	int i = 0;
	
	while(i < 4){
		cin >> A[i];
		i++;
	}

	switch(A[0]){
		case 0: // A[0] = addi A[1] = rd, A[2] = rs1, A[3] = imm
			instruction.I_s.opcode = 0x13;
			instruction.I_s.rd = A[1];
			instruction.I_s.funct3 = 0x0;
			instruction.I_s.rs1 = A[2];
			instruction.I_s.imm = A[3];
			break;
		case 1: // A[0] = lw, A[1] = rd, A[2] = rs1, A[3] = imm
			instruction.I_s.opcode = 0x03;
			instruction.I_s.rd = A[1];
			instruction.I_s.funct3 = 0x02;
			instruction.I_s.rs1 = A[2];
			instruction.I_s.imm = A[3];
			break;
		case 2: // A[0] = sw, A[1] = rs2, A[2] = rs1, A[3] = imm
			instruction.S_s.opcode = 0x23;
			instruction.S_s.rs2 = A[1];
			instruction.S_s.funct3 = 0x02;
			instruction.S_s.rs1 = A[2];
			instruction.S_s.imm4_0 = A[3]; // this should be kept below 15 (yes this is shitty Irene, but it is just for debugging)
			break;
		case 3: // A[0] = add, A[1] = rs2, A[2] = rs1, A[3] = rs2
			instruction.R_s.opcode = 0x33;
			instruction.R_s.rd = A[1];
			instruction.R_s.funct3 = 0x0;
			instruction.R_s.rs1 = A[2];
			instruction.R_s.rs2 = A[3];
			instruction.R_s.funct7 = 0x0;
			break;
		case 4: // A[0] = sra, A[1] = rs2, A[2] = rs1, A[3] = rs2
			instruction.R_s.opcode = 0x33; //011 0011 
			instruction.R_s.rd = A[1];
			instruction.R_s.funct3 = 0x5; //101
			instruction.R_s.rs1 = A[2];
			instruction.R_s.rs2 = A[3];
			instruction.R_s.funct7 = 0x20; //010 0000 
			break;
		case 5:
			instruction.B_s.opcode = 0x73;
		default : 
			break;
	}

	return instruction.instruction;

}

//111111111111111000000010
//11111111000000000000000111111110

uint32_t R(InstructionUnion instruction){ //not done yet, I got distracted -ID
	uint32_t encoding =  ((uint32_t)(instruction.R_s.funct7) << 10) | ((uint32_t)(instruction.R_s.funct3) << 7) | instruction.R_s.opcode; // funct7, funct3 and opcode informs us what instruction we are dealing with
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
			cout << "Not a recognized R-type instruction" << endl;
			break;
	}
	return 0;
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
			Reg[instruction.I_s.rd] = Memory[instruction.I_s.imm + instruction.I_s.rs1] | signExtend(Memory[instruction.I_s.imm + instruction.I_s.rs1], byte-1);
			break;
		case 0x83: // LH - 1000 0011
			Reg[instruction.I_s.rd] = (Memory[instruction.I_s.imm + instruction.I_s.rs1 + 1] << byte) | Memory[instruction.I_s.imm + instruction.I_s.rs1] | signExtend(((Memory[instruction.I_s.imm + instruction.I_s.rs1] << byte) | Memory[instruction.I_s.imm + instruction.I_s.rs1]), 2*byte-1);
			break;
		case 0x103: //LW - 0001 0000 0011
			Reg[instruction.I_s.rd] = ((uint32_t)Memory[instruction.I_s.imm + instruction.I_s.rs1 + 3] << 3*byte) | ((uint32_t)Memory[instruction.I_s.imm + instruction.I_s.rs1 + 2] << 2*byte) | ((uint32_t)Memory[instruction.I_s.imm + instruction.I_s.rs1 + 1] << byte) | (uint32_t)Memory[instruction.I_s.imm + instruction.I_s.rs1];
			break;
		case 0x203: //LBU - 0010 0000 0011 
			Reg[instruction.I_s.rd] = Memory[instruction.I_s.imm + instruction.I_s.rs1];
			break;
		case 0x283: // LHU - 0010 1000 0011
			Reg[instruction.I_s.rd] = (Memory[instruction.I_s.imm + instruction.I_s.rs1 + 1] << byte) | Memory[instruction.I_s.imm + instruction.I_s.rs1];
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
	}
	return 0;
}


uint32_t S(InstructionUnion instruction){ 
	int imm = signExtend(((instruction.S_s.imm11_5) << 5) | instruction.S_s.imm4_0,11); 

	cout << "immediate is " << imm << endl;
	cout << "Gonna try to save in Memory[" << (int)signExtend(Reg[instruction.S_s.rs1],11) << " + " << imm << "]" << endl;
    switch(instruction.S_s.funct3){
    	case 0x0:	// SB - 000
    		Memory[(int)signExtend(Reg[instruction.S_s.rs1],11) + imm 	 ] =  Reg[instruction.S_s.rs2] 			& 0xFF; // Only stores the first byte
    		break;
    	case 0x1:  	// SH - 001
    		Memory[(int)signExtend(Reg[instruction.S_s.rs1],11) + imm 	 ] =  Reg[instruction.S_s.rs2] 			& 0xFF; // First byte
    		Memory[(int)signExtend(Reg[instruction.S_s.rs1],11) + imm + 1] = (Reg[instruction.S_s.rs2] >> byte) & 0xFF; // Second byte     	 
    		break;
    	case 0x2:	// SW - 010
	   		Memory[(int)signExtend(Reg[instruction.S_s.rs1],11) + imm 	 ] =   Reg[instruction.S_s.rs2] & 0xFF; 					 // First byte
    		Memory[(int)signExtend(Reg[instruction.S_s.rs1],11) + imm + 1] = ((Reg[instruction.S_s.rs2] & 0xFF00) 	  >>   byte); // Second byte
    		Memory[(int)signExtend(Reg[instruction.S_s.rs1],11) + imm + 2] = ((Reg[instruction.S_s.rs2] & 0xFF0000)   >> 2*byte); // Third byte
    		Memory[(int)signExtend(Reg[instruction.S_s.rs1],11) + imm + 3] = ((Reg[instruction.S_s.rs2] & 0xFF000000) >> 3*byte); // What do you think byte
    		break;
    	default :
			cout << "Not a recognized S-type instruction" << endl;
    		break;
    }

   return 0;
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
			break;
	}
	return 0;
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
    		break;
    }
    return 0;
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
    		break;
    }
    return 0;
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


int main(){
	bool flag = false, notAtTheEnd = true;
	InstructionUnion instruction;
	char instructionType;
	uint32_t branchInstruction = 0;
	initRegister();
	setMemoryToZero();
	//shittyInput();
	if(readFileIntoMemory()){
		return 0;
	}
	printMemory();
	printProgram(pcmax);
	while(notAtTheEnd){
		cout << "started execution loop" << endl;
		instruction.instruction = Memory[pc] | Memory[pc+1] << byte | Memory[pc + 2] << 2*byte | Memory[pc + 3] << 3*byte;


		instructionType = whatKindOfInstruction(instruction);

		switch(instructionType){
			case 'R':
				R(instruction);
				break;
			case 'I':
				I(instruction);
				break;
			case 'S':
				S(instruction);
				break;
			case 'U':
				U(instruction);
				break;
			case 'B':
				B(instruction);
				break;
			case 'J':
				J(instruction);
				break;
			case 'X':
				cout << "closing" << endl;
				notAtTheEnd = false;
				break;
			default:
				flag = false;
				cout << "Invalid input" << endl;
				break;		
		}
		cout << "pc = " << dec << pc << endl;
		if(flag){ // This just ensures that it doesn't print everything if it doesn't get a valid input.
			printRegister();
			printMemory();	
		}
		//flag = true;
		pc += 4;
	}
	printMemory();
	printRegister();


	return 0;
}

/*
List of tests:
addlarge  - success
addneg - success	
addpos - success
shift - success
*/