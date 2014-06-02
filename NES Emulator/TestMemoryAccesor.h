#pragma once
#include "memoryaccesor.h"
class TestMemoryAccesor :
	public MemoryAccesor
{
public:
	TestMemoryAccesor(void);
	virtual uint8_t GetMemory(uint16_t addr) const;
	virtual void SetMemory(uint16_t addr, uint8_t value);

private:
	uint8_t _memory[0x10000];

};

