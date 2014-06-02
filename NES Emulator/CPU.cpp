#include "stdafx.h"
#include "CPU.h"
#include "Opcodes.h"


const std::unordered_map<int,Opcode> CPU::Opcodes = CPU::CreateOpcodes();

std::unordered_map<int,Opcode> CPU::CreateOpcodes(){
	std::unordered_map<int,Opcode> opcodes;

	//ADC
	opcodes[0x69] = Opcode(ADC,Immediate);
	opcodes[0x65] = Opcode(ADC,ZeroPage);
	opcodes[0x75] = Opcode(ADC,ZeroPageX);
	opcodes[0x6D] = Opcode(ADC,Absolute);
	opcodes[0x7D] = Opcode(ADC,AbsoluteX);
	opcodes[0x79] = Opcode(ADC,AbsoluteY);
	opcodes[0x61] = Opcode(ADC,IndirectX);
	opcodes[0x71] = Opcode(ADC,IndirectY);

	//AND
	opcodes[0x29] = Opcode(AND,Immediate);
	opcodes[0x25] = Opcode(AND,ZeroPage);
	opcodes[0x35] = Opcode(AND,ZeroPageX);
	opcodes[0x2D] = Opcode(AND,Absolute);
	opcodes[0x3D] = Opcode(AND,AbsoluteX);
	opcodes[0x39] = Opcode(AND,AbsoluteY);
	opcodes[0x21] = Opcode(AND,IndirectX);
	opcodes[0x31] = Opcode(AND,IndirectY);

	//ASL
	opcodes[0x0A] = Opcode(ASL,Implied);
	opcodes[0x06] = Opcode(ASL,ZeroPage);
	opcodes[0x16] = Opcode(ASL,ZeroPageX);
	opcodes[0x0E] = Opcode(ASL,Absolute);
	opcodes[0x1E] = Opcode(ASL,AbsoluteX);

	//BIT
	opcodes[0x24] = Opcode(BIT,ZeroPage);
	opcodes[0x2C] = Opcode(BIT,Absolute);

	//BRANCHING
	opcodes[0x10] = Opcode(BPL, Relative);
	opcodes[0x30] = Opcode(BMI, Relative);
	opcodes[0x50] = Opcode(BVC, Relative);
	opcodes[0x70] = Opcode(BVS, Relative);
	opcodes[0x90] = Opcode(BCC, Relative);
	opcodes[0xB0] = Opcode(BCS, Relative);
	opcodes[0xD0] = Opcode(BNE, Relative);
	opcodes[0xF0] = Opcode(BEQ, Relative);


	return opcodes;
}

uint16_t CPU::GetAddressingLoc(AddressingMode am, uint8_t arg1, uint8_t arg2) const{
	switch(am){
	case ZeroPage:
		return arg1;
	case ZeroPageX:
		return ((uint16_t)arg1 + (uint16_t)_X)%0x100;
	case ZeroPageY:
		return ((uint16_t)arg1 + (uint16_t)_Y)%0x100;
	case Absolute:
		return (uint16_t)arg1 + ((uint16_t)arg2)<<8;
	case AbsoluteX:
		return (uint16_t)arg1 + ((uint16_t)arg2)<<8 + _X;
	case AbsoluteY:
		return (uint16_t)arg1 + ((uint16_t)arg2)<<8 + _Y;
	case Indirect:
		{
		uint16_t indirectAddr = (uint16_t)arg1 + ((uint16_t)arg2)<<8;
		return (uint16_t)_memAccessor->GetMemory(indirectAddr) +
			((uint16_t)_memAccessor->GetMemory(indirectAddr+1))<<8;
		}
	case IndirectX: //STA ($44,X) 
		{
		uint16_t zeroPageAddr = ((uint16_t)arg1 + (uint16_t)_X)%0x100;
		return (uint16_t)_memAccessor->GetMemory(zeroPageAddr) +
			((uint16_t)_memAccessor->GetMemory(zeroPageAddr+1))<<8;
		}
	case IndirectY: //STA ($44),Y 
		uint8_t lsb = _memAccessor->GetMemory(arg1);
		uint8_t msb = _memAccessor->GetMemory(arg1+1);
		uint16_t base = (uint16_t)lsb + ((uint16_t)msb)<<8;
		return base + _Y;
	}
}

//TODO FUTURE: account for cycles
void CPU::Step(){

	uint8_t operand = _memAccessor->GetMemory(_pc);
	auto opcode = (*Opcodes.find(operand)).second;

	uint8_t arg1 = _memAccessor->GetMemory(_pc+1);
	uint8_t arg2 = _memAccessor->GetMemory(_pc+2);

	_pc += opcode.Bytes;

	uint16_t memoryArg = 0;

	if(opcode.ReferencesMemory()){
		memoryArg = GetAddressingLoc(opcode.Addressing,arg1,arg2);
	}

	if(opcode.Mnemonic == ADC){
		uint16_t accumulated = _A;
		uint16_t addOn;

		if(opcode.Addressing == Immediate){
			addOn = arg1;
		}
		else{
			addOn = _memAccessor->GetMemory(memoryArg);
		}

		bool sameSign = ~(addOn^_A) & (1<<7) != 0;

		accumulated += addOn;
		accumulated += _carryFlag? 1:0;

		_A = accumulated % 0x100;

		//A' = A + whatever. overflow set if  A and whatever have same sign, each different than sign of A'
		_overflowFlag = sameSign && ((_A^addOn) & (1<<7) != 0);
		_carryFlag = accumulated > 0xFF;
		_zeroFlag = _A == 0;
		_negativeFlag = _A&(1<<7) != 0;
	}

	else if (opcode.Mnemonic == AND){
		if(opcode.Addressing == Immediate){
			_A &= arg1;
		}
		else{
			_A &= _memAccessor->GetMemory(memoryArg);
		}

		_zeroFlag = _A == 0;
		_negativeFlag = _A&(1<<7) != 0;
	}
	
	else if (opcode.Mnemonic == ASL){
		_carryFlag = _A & (1<<7) != 0;

		if(opcode.Addressing == Implied){
			_A <<=2;
		}
		else{
			_A <<= _memAccessor->GetMemory(memoryArg);
		}

		_zeroFlag = _A == 0;
		_negativeFlag = _A&(1<<7) != 0;
	}

	
	else if(opcode.Mnemonic == BIT){
		uint8_t memVal = _memAccessor->GetMemory(memoryArg);
		uint8_t hypotheticalAnd = _A & memVal;
		_overflowFlag = memVal & (1<<6) != 0;
		_carryFlag =  memVal & (1<<7) != 0;
	}

	else if (opcode.Mnemonic == BCC){
		if(!_carryFlag)
			_pc += (int8_t)arg1;
	}

	else if (opcode.Mnemonic == BCS){
		if(_carryFlag)
			_pc += (int8_t)arg1;
	}

	else if (opcode.Mnemonic == BEQ){
		if(_zeroFlag)
			_pc += (int8_t)arg1;
	}

	else if (opcode.Mnemonic == BNE){
		if(!_zeroFlag)
			_pc += (int8_t)arg1;
	}

	else if (opcode.Mnemonic == BMI){
		if(_negativeFlag)
			_pc += (int8_t)arg1;
	}

	else if (opcode.Mnemonic == BPL){
		if(!_negativeFlag)
			_pc += (int8_t)arg1;
	}

	else if (opcode.Mnemonic == BVC){
		if(!_overflowFlag)
			_pc += (int8_t)arg1;
	}

	else if (opcode.Mnemonic == BVS){
		if(_overflowFlag)
			_pc += (int8_t)arg1;
	}

	
	//TODO: MOAR OPCODES
}