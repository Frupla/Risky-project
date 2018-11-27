#ifndef FILEINOUT
#define FILEINOUT


using namespace std;
typedef unsigned char BYTE;

extern uint32_t Reg[32]; 		// The 32 registers
extern uint8_t Memory[1<<20]; // the memory, an array of bytes of length 2^20
extern uint32_t pc; 		// the program counter
extern uint32_t pcmax;


string getFileName();
void writeMemoryIntoFile(string inputname);
void writeRegisterIntoFile(string inputname);
void writeProgramIntoFile(string inputname);
bool readFileIntoMemory(string inputname);
long getFileSize(FILE *file);
void printResult(string inputname);
void printExpected(string inputname);




#endif