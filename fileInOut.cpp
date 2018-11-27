#include <iostream>
#include <stdint.h>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <cstring>

#include "fileInOut.h"

using namespace std;



string getFileName(){
	cout << "The name of the .bin file you want to execute (just the name, not .bin)" << endl;
	string filename;
	cin >> filename;
	return filename;
}

void writeMemoryIntoFile(string inputname){
	string outputfilename = inputname + "_mem.bin";
	int k = 0;

	ofstream file(outputfilename, ios::binary);
	while(k < 1<<20){
		file.write((char*)&(Memory[k]), sizeof(uint32_t));
		k++;
	}
}

void writeRegisterIntoFile(string inputname){
	string outputfilename = inputname + "_res.bin";
	int k = 0;

	ofstream file(outputfilename, ios::binary);
	while(k < 32){
		file.write((char*)&(Reg[k]), sizeof(uint32_t));
		k++;
	}
}

void writeProgramIntoFile(string inputname){
	string outputfilename = inputname + "_pro.bin";
	int k = 0;

	ofstream file(outputfilename, ios::binary);
	while(k < pcmax){
		file.write((char*)&(Memory[k]), sizeof(uint32_t));
		k++;
	}
}

bool readFileIntoMemory(string inputname){
	string filename = inputname + ".bin";
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



// Get the size of a file - Shady code I found on the web
long getFileSize(FILE *file){
    long lCurPos, lEndPos;
    lCurPos = ftell(file);
    fseek(file, 0, 2);
    lEndPos = ftell(file);
	fseek(file, lCurPos, 0);
    return lEndPos;
}

void printResult(string inputname){
	inputname = inputname + "_res.bin";

	int n = inputname.length();

	int skip = 7;

	char * char_array;

	char_array = new char[n+1];

	for(int i = 0; i < n; i++){
		char_array[i] = inputname[i];
	}


    const char *filePath = char_array; 
    BYTE *fileBuf;          // Pointer to our buffered data
    FILE *file = NULL;      // File pointer

	// Open the file in binary mode using the "rb" format string
    // This also checks if the file exists and/or can be opened for reading correctly
	if ((file = fopen(filePath, "rb")) == NULL){
		cout << char_array << " not found" << endl;
	}
    
    else{
		cout << "Printing from " << char_array << endl;
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


	    delete[]fileBuf;
	    fclose(file);   // Almost forgot this
	}
}


void printExpected(string inputname){

	inputname = inputname + ".res";

	int n = inputname.length();

	int skip = 7;

	char * char_array;

	char_array = new char[n+1];

	for(int i = 0; i < n; i++){
		char_array[i] = inputname[i];
	}


    const char *filePath = char_array; 
    BYTE *fileBuf;          // Pointer to our buffered data
    FILE *file = NULL;      // File pointer

	// Open the file in binary mode using the "rb" format string
    // This also checks if the file exists and/or can be opened for reading correctly
	if ((file = fopen(filePath, "rb")) == NULL){
		cout << char_array << " not found" << endl;
	}
    
    else{
		cout << "Printing from " << char_array << endl;
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


	    delete[]fileBuf;
	    fclose(file);   // Almost forgot this
	}
}
