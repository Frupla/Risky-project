#include <iostream>
#include <stdint.h>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <cstring>
#include "riscvInstructionHandler.h"
#include "fileInOut.h"

using namespace std;


int main(){
	uint32_t notAtTheEnd = 1;
	InstructionUnion instruction;
	char instructionType;
	uint32_t branchInstruction = 0;
	string input = getFileName();
	initRegister();
	setMemoryToZero();
	if(readFileIntoMemory(input)){
		return 0;
	}
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
		//printRegister();
		pc += 4;
	}
	writeRegisterIntoFile(input);
	writeMemoryIntoFile(input);
	printResult(input);
	printExpected(input);

	return 0;
}
