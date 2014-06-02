#include "stdafx.h"
#include "TestMemoryAccesor.h"


TestMemoryAccesor::TestMemoryAccesor(void)
{
}

 uint8_t TestMemoryAccesor::GetMemory(uint16_t addr) const{
	return _memory[addr];
}

void TestMemoryAccesor::SetMemory(uint16_t addr, uint8_t value){
	_memory[addr] = value;
}