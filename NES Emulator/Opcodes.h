#pragma once

enum Mnemonic{
	ADC,
	AND,
	ASL,
	BIT,
	BCC,
	BCS,
	BEQ,
	BMI,
	BPL,
	BVC,
	BVS,
	BNE
};

enum AddressingMode{
	Immediate = 1,
	ZeroPage = 1<<2,
	ZeroPageX = 1<<3,
	ZeroPageY = 1<<4,
	Absolute = 1<<5,
	AbsoluteX = 1<<6,
	AbsoluteY = 1<<7,
	IndirectX = 1<<8,
	IndirectY = 1<<9,
	Indirect = 1<<10,
	Implied = 1<<11,
	Relative = 1<<12
};

struct Opcode{
	Opcode(){}
	Opcode(Mnemonic m, AddressingMode a, int cycles=0):Mnemonic(m),Addressing(a),Cycles(cycles){
		if( (a & Implied) != 0){
			Bytes=1;
		}
		else if( (a & Immediate|ZeroPage|ZeroPageX|ZeroPageY|Relative) !=0){
			Bytes = 2;
		}
		else{
			Bytes = 3;
		}
	}

	bool ReferencesMemory() const{
		return (Addressing & (Immediate|Implied|Relative)) == 0;
	}
	Mnemonic Mnemonic;
	AddressingMode Addressing;
	int Bytes;
	int Cycles;
};
