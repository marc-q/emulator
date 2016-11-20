#ifndef __EMULATOR_H__
#define __EMULATOR_H__

//
//TYPES
//
typedef unsigned short word;
typedef signed short sword;
typedef unsigned char byte;
typedef signed char sbyte;

//
// CPU
//
#define E_MAX_SRAM 512
#define E_MAX_FLASH 128

#define E_PORT_STDIO 0x52
#define E_PORT_EXIT 0x4F

// Status register
// NAME		DESCRIPTION
// *C		Carry-Flag
// Z		Last operation == 0
// N		Last operation is negative
// *O		Overflow-Flag
// END		!!INTERNAL USE ONLY!!
// *: Currently not implemented!
enum e_cpu_sregs
{
	CPU_SREG_Z = 0,
	CPU_SREG_N,
	CPU_SREG_END
};

// REGISTERS
// NAME		DESCRIPTION
// RSP		!!STACK POINTER!!
// R8		Usable by user
// R9		Usable by user
// R10		Usable by user
// R11		Usable by user
// R12		Usable by user
// R13		Usable by user
// R14		Usable by user
// R15		Usable by user
// END		!!INTERNAL USE ONLY!!
enum e_cpu_regs
{
	CPU_REG_RAX = 0,
	CPU_REG_RBX,
	CPU_REG_RCX,
	CPU_REG_RDX,
	CPU_REG_RBP,
	CPU_REG_RSI,
	CPU_REG_RDI,
	CPU_REG_RSP,
	
	CPU_REG_R8,
	CPU_REG_R9,
	CPU_REG_R10,
	CPU_REG_R11,
	CPU_REG_R12,
	CPU_REG_R13,
	CPU_REG_R14,
	CPU_REG_R15,
	
	CPU_REG_END
};

// OP CODES
// OP CODE	DESCIPTION					ARGS
// BAD		!!BAD INSTRUCTION!!
// LDI		Load <b> into register <a>			a,b
// MOV		Move <b> into register <a>			a,b
// MOVW		Move word <b> to word <a>			a,b

// PUSH		Add <a> to the stack				a
// POP		Set <a> to the top of the stack			a

// ADIW		Add <a> to register <b>				a,b
// ADD		Add register <b> to register <a>		a,b
// SUBI		Subtract <b> from register <a>			a,b
// SUB		Subtract register <b> from register <a>		a,b
// MUL		Multiply register <a> with register <b>		a,b
// *DIV		Divide register <a> by register <b>		a,b
// INC		Increment register <a>				a
// DEC		Decrement register <a>				a

// SWAP		Swap register <a>				a
// NEG		Negate register <a>				a
// ROR		Rotate register <a>				a

// ANDI		register <a> = register <a> & <b>		a,b
// AND		register <a> = register <a> & register <b>	a,b
// ORI		register <a> = register <a> | <b>		a,b
// OR		register <a> = register <a> | register <b>	a,b
// EOR		register <a> = register <a> ^ register <b>	a,b

// CPI		Set the EQUAL flag if reg <a> equals <b>	a,b
// CP		Set the EQUAL flag if reg <a> equals reg <b>	a,b 

// JE		Set the instruction ptr to <a> if EQUAL		a
// JNE		Set the instruction ptr to <a> if NOT EQUAL	a
// JZ		Set the instruction ptr to <a> if NULL		a

// BREAK	Quit the program
// NOP		Do nothing
// OUT		Output register <b> on register <a>		a,b
// IN		Input -> register <b> to register <a>		a,b
// END		!!INTERNAL USE ONLY!!
// *: Currently not implemented!
enum e_cpu_opcode
{
	CPU_OPC_BAD = 0,
	CPU_OPC_LDI,
	CPU_OPC_MOV,
	CPU_OPC_MOVW,
	
	CPU_OPC_PUSH,
	CPU_OPC_POP,
	
	CPU_OPC_ADIW,
	CPU_OPC_ADD,
	CPU_OPC_SUBI,
	CPU_OPC_SUB,
	CPU_OPC_MUL,
	CPU_OPC_INC,
	CPU_OPC_DEC,
	
	CPU_OPC_SWAP,
	CPU_OPC_NEG,
	CPU_OPC_ROR,
	
	CPU_OPC_ANDI,
	CPU_OPC_AND,
	CPU_OPC_ORI,
	CPU_OPC_OR,
	CPU_OPC_EOR,
	
	CPU_OPC_CPI,
	CPU_OPC_CP,
	
	CPU_OPC_JE,
	CPU_OPC_JNE,
	CPU_OPC_JZ,
	
	CPU_OPC_BREAK,
	CPU_OPC_NOP,
	CPU_OPC_OUT,
	CPU_OPC_IN,
	CPU_OPC_END
};

struct _e_cpu
{
	// SRAM
	byte sram[E_MAX_SRAM];
	
	// FLASH
	// program counter
	unsigned int pc;
	word flash[E_MAX_FLASH];
	
	// Status register
	bool sreg[CPU_SREG_END];
};

typedef struct _e_cpu e_cpu;

#endif /* __EMULATOR_H__ */
