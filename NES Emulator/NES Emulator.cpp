// NES Emulator.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "CPU.h"
#include "TestMemoryAccesor.h"
#include <memory>

void AdcTest(){

	uint8_t program[] = {
		//0x05, //junk byte since PC is incremented before reading instruction
		0x69, 0x16, // LDA #$16
		0x6D ,0x00 ,0x00 //LDA $00
		
	};

	auto tma =std::unique_ptr<TestMemoryAccesor>(new TestMemoryAccesor());
	for(int i = 0; i<sizeof(program); i++){
		tma->SetMemory(i,program[i]);
	}
	CPU cpu(std::move(tma));
	cpu.Step(); // A = 22
	cpu.Step(); // A = 22 + *(0) = 22 + 0x69 = 22 + 105 = 127

}
int _tmain(int argc, _TCHAR* argv[])
{
	AdcTest();
	uint16_t jim = 500;
	uint8_t bob = 250;
	jim += (int8_t)bob;
	return 0;
}

