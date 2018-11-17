
#include <iostream>
#include <stdint.h>
#include <bitset>  
using namespace std; 


//conclusion: negative numbers shift arimetically

int main(){
	uint8_t i = 0xf0;
	uint8_t j = 0;
	std::cout << std::hex << 'i' << (uint32_t)i << 'j' << (uint32_t)j <<'\n';
	j = i << 2;
	std::cout << 'i' << (uint32_t)i << 'j' << (uint32_t)j <<'\n';
	j = i >> 2;
	std::cout << 'i' << (uint32_t)i << 'j' << (uint32_t)j <<'\n';
	uint32_t ii = -4;
	uint32_t jj;
	std::cout << std::hex << 'i' << ii << 'j' << jj <<'\n';
	jj = ii << 2;
	std::cout << 'i' << ii << 'j' << jj <<'\n';
	jj = ii >> 2;
	std::cout << 'i' << ii << 'j' << jj <<'\n';
	return 0;
}	
/*
i: f0 j: 0, f0 = 1111 0000, 0 = 0000 0000
i: f0 j: c0,f0 = 1111 0000,c0 = 1100 0000
i: f0 j: 3c,f0 = 1111 0000, j = 0011 1100
int:
ii: fffffffc jj: 0
ii: fffffffc jj: fffffff0
ii: fffffffc jj: ffffffff
uint32_t
i: fffffffc j: 0
i: fffffffc j: fffffff0
i: fffffffc j: 3fffffff
*/