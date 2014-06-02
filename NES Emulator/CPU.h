#pragma once

#include "MemoryAccesor.h"
#include <unordered_map>
#include "Opcodes.h"
#include <memory>

class CPU
{
public:
	explicit CPU(std::unique_ptr<MemoryAccesor> memAccessor, uint16_t pc=0):_pc(pc),
	_sp(0),_A(0),_X(0),_Y(0),_zeroFlag(false),_carryFlag(false),_negativeFlag(false),
	_overflowFlag(false),_interruptDisableFlag(false), _memAccessor(std::move(memAccessor)){
	}

	void Step();
private:
	std::unique_ptr<MemoryAccesor> _memAccessor;

	static const std::unordered_map<int,Opcode> Opcodes;
	uint16_t _pc;
	uint8_t _sp;

	uint8_t _A,_X,_Y;

	bool _zeroFlag;
	bool _carryFlag;
	bool _negativeFlag;
	bool _overflowFlag;
	bool _interruptDisableFlag;

	static std::unordered_map<int,Opcode> CreateOpcodes();
	uint16_t GetAddressingLoc(AddressingMode am, uint8_t arg1, uint8_t arg2) const;

};

