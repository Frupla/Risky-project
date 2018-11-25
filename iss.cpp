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
static int lengthOfMemory = 1<<20;
uint32_t Reg[32]; 		// The 32 registers
uint32_t pc = 0; 		// the program counter
uint8_t Memory[1<<20]; // the memory, an array of bytes of length 2^20
uint32_t pcmax = 0;
string resfile;

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
	cout << "Printing register (skipping zeros)" << endl;
	for(int i = 0; i < 32; i++){
		if(Reg[i] != 0){
			cout << "x" << dec << i << " = " << dec << (int)Reg[i] << endl;
		}
	}
	cout << "-----" << endl;
}

void printRegisterSquare(){
	cout << "Printing register in little endian square" << endl;
	cout << "(Ignore x2, it's the stack pointer)" << endl;
	int k = 0;
	for (int i = 0; i < 8;i++)
	{
		for(int j = 0; j < 4; j++){
				printf("%02x%02x %02x%02x ",(Reg[k] & 0xff),((Reg[k]>>byte) & 0xff),((Reg[k]>>2*byte) & 0xff),((Reg[k]>>3*byte) & 0xff)); //%02x ntoh hton
				k++;
		}
		printf("\n");
	}
	cout << "-----" << endl;
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
	cout << "Write a filename, BUT not .bin, what would fuck everything up" << endl;
	string filename, input;
	cin >> input;
	filename = input + ".bin";
	resfile = input + ".res";
	int i = 0;
	streampos fileSize;

	char * temporaryMemory;
	temporaryMemory = new char [2];

	ifstream file (filename, ios::in|ios::binary|ios::ate);//open file and set pointer at end of file
	if (file.is_open())
	{
		cout << "file is open" << endl;
		fileSize = file.tellg(); //use pointer to get file size
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
	}else {
		cout << "Unable to open file" << endl;
		return 1;
	}
	pcmax = i;	
	return 0;
}


typedef unsigned char BYTE;

// Get the size of a file - Shady code I found on the web
long getFileSize(FILE *file){
    long lCurPos, lEndPos;
    lCurPos = ftell(file);
    fseek(file, 0, 2);
    lEndPos = ftell(file);
	fseek(file, lCurPos, 0);
    return lEndPos;
}
void printResfile(){

	int n = resfile.length();

	int skip = 7;

	char * char_array;

	char_array = new char[n+1];

	for(int i = 0; i < n; i++){
		char_array[i] = resfile[i];
	}

    const char *filePath = char_array; 
    BYTE *fileBuf;          // Pointer to our buffered data
    FILE *file = NULL;      // File pointer

	// Open the file in binary mode using the "rb" format string
    // This also checks if the file exists and/or can be opened for reading correctly
	if ((file = fopen(filePath, "rb")) == NULL){
		cout << "Could not find .res file, are you sure it is in the directory?" << endl;
	}
    
    else{
		cout << "Printing from the .res file" << endl;
	    // Get the size of the file in bytes
	    long fileSize = getFileSize(file);

	    // Allocate space in the buffer for the whole file
	    fileBuf = new BYTE[fileSize];

	    // Read the file in to the buffer
	    fread(fileBuf, fileSize, 1, file);
	 
	    // Now that we have the entire file buffered, we can take a look at some binary infomation
	    // Lets take a look in hexadecimal
	    for (int i = 0; i < fileSize; i += 2){
	        printf("%02x%02x ", fileBuf[i], fileBuf[i+1]);

	        if(!skip){
	        	cout << endl;
	        	skip = 8;
	        }
	        skip--;
	    }


	    cin.get();
	    delete[]fileBuf;
	    fclose(file);   // Almost forgot this
    }
}


uint32_t R(InstructionUnion instruction){ //not done yet, I got distracted -ID
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
		case 0x00433:// MUL 0 0000 0100 1011 0011 = 0x0433
			Reg[instruction.R_s.rd] = (int)Reg[instruction.R_s.rs1] * (int)Reg[instruction.R_s.rs2];
			break;
		case 0x004b3:// MULH 0 0000 0111 1011 0011 = 0x04b3
			signedLongVar = (int64_t)Reg[instruction.R_s.rs1] * (int64_t)Reg[instruction.R_s.rs2];
			cout << hex << "signedLongVar= " << signedLongVar << endl;
			signedVar = (int32_t)((signedLongVar & 0x00000000ffffffff) >> 32); //& 0x00000000ffffffff);
			cout << hex << "signedVar= " << signedVar << endl;
			Reg[instruction.R_s.rd] = (int)signedVar;
			//Reg[instruction.R_s.rd] = (uint32_t)(((int64_t)Reg[instruction.R_s.rs1] * (int64_t)Reg[instruction.R_s.rs2]) >> 4*byte);

			break;
		case 0x00533:// MULHSU 0 0000 0101 0011 0011 = 0x0533
			
			break;
		case 0x005b3:// MULHU 0 0000 0101 1011 0011 = 0x05b3
			
			break;
		case 0x00633:// DIV 0 0000 0110 0011 0011 = 0x0633
			//Reg[instruction.R_s.rd] = (int)Reg[instruction.R_s.rs1] / (int)Reg[instruction.R_s.rs2];
			break;
		case 0x006b3:// DIVU 0 0000 0110 1011 0011 = 0x06b3
			//Reg[instruction.R_s.rd] = Reg[instruction.R_s.rs1] / Reg[instruction.R_s.rs2];
			break;
		case 0x00733:// REM 0 0000 0111 0011 0011 = 0x0733
			
			break;
		case 0x007b3:// REMU 0 0000 0111 1011 0011 = 0x07b3
			
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
			cout << "closing" << endl;
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


int main(){
	uint32_t notAtTheEnd = 1;
	InstructionUnion instruction;
	char instructionType;
	uint32_t branchInstruction = 0;
	initRegister();
	setMemoryToZero();
	if(readFileIntoMemory()){
		return 0;
	}
//	printProgram(pcmax);
	while(notAtTheEnd && pc < pcmax){
		instruction.instruction = Memory[pc] | Memory[pc+1] << byte | Memory[pc + 2] << 2*byte | Memory[pc + 3] << 3*byte;

		instructionType = whatKindOfInstruction(instruction);
		
		switch(instructionType){
			case 'R':
				if(R(instruction)){;}
				else{
					cout << "R-type instruction failed" << endl;
					cout << hex << "The instruction was: " << instruction.instruction << endl;
					notAtTheEnd = 0;
				}
				break;
			case 'I':
				if(I(instruction)){;}
				else{
					cout << "I-type instruction failed" << endl;
					cout << hex << "The instruction was: " << instruction.instruction << endl;
					notAtTheEnd = 0;
				}
				break;
			case 'S':
				if(S(instruction)){;}
				else{
					cout << "S-type instruction failed" << endl;
					cout << hex << "The instruction was: " << instruction.instruction << endl;
					notAtTheEnd = 0;
				}
				break;
			case 'U':
				if(U(instruction)){;}
				else{
					cout << "U-type instruction failed" << endl;
					cout << hex << "The instruction was: " << instruction.instruction << endl;
					notAtTheEnd = 0;
				}
				break;
			case 'B':
				if(B(instruction)){;}
				else{
					cout << "B-type instruction failed" << endl;
					cout << hex << "The instruction was: " << instruction.instruction << endl;
					notAtTheEnd = 0;
				}
				break;
			case 'J':
				if(J(instruction)){;}
				else{
					cout << "J-type instruction failed" << endl;
					cout << hex << "The instruction was: " << instruction.instruction << endl;
					notAtTheEnd = 0;
				}
				break;
			case 'X':
				if(X()){;}
				else{
					notAtTheEnd = 0;
				}
				break;
			default:
				cout << "Invalid input" << endl;
				cout << hex << "opcode was " << instruction.B_s.opcode << " and funct3 is " << instruction.R_s.funct3 << " (might not be relevant)" << endl;
				notAtTheEnd = 0;
				break;		
		}
		Reg[x0] = 0; // Can't be changed mofo
		//cout << "pc = " << dec << pc << endl;
		printRegister();
		pc += 4;
	}
	//printMemory();
	printRegister();
	printRegisterSquare();
	printResfile();

	return 0;
}
