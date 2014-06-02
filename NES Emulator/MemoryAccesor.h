#pragma once

#include <cstdint>

class MemoryAccesor
{
public:
	MemoryAccesor(void);
	virtual uint8_t GetMemory(uint16_t addr) const = 0;
	virtual void SetMemory(uint16_t addr, uint8_t value) = 0;

};

