/* Copyright 2016 Marc Volker Dickmann */
/* Project: Emulator */
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "emulator.h"

#define __DEBUG__ 1

//
// CPU
//
static void e_cpu_init (e_cpu *a, const word *prg)
{
	int i;
	
	// SRAM
	for (i = 0; i < E_MAX_SRAM; i++)
	{
		a->sram[i] = 0;
	}
	
	// FLASH
	for (i = 0; i < E_MAX_FLASH; i++)
	{
		if (prg[i] == '\0')
		{
			break;
		}
		
		a->flash[i] = prg[i];
	}
	
	// Status register
	a->sreg[CPU_SREG_Z] = true;
	a->sreg[CPU_SREG_N] = false;
}

static void e_cpu_free (e_cpu *a)
{
	printf ("\n");
	// CLEANING
}

static void e_cpu_update (e_cpu *a, const word vres)
{
	// Update the status register FLAGS
	a->sreg[CPU_SREG_Z] = vres == 0;
	a->sreg[CPU_SREG_N] = vres & 0x80;
}

//
// SRAM
//
static inline word e_cpu_sram_read (e_cpu *a, const unsigned int addr)
{
	return a->sram[addr] | (a->sram[addr + 1] << 8);
}

static inline void e_cpu_sram_write (e_cpu *a, const unsigned int addr, const word value)
{
	a->sram[addr] = value & 0x00FF;
	a->sram[addr + 1] = value >> 8;
}

//
// OP CODE
//
static inline word e_cpu_get_arg (e_cpu *a)
{
	// To allow args with more than 8 bit length!
	return a->flash[++a->pc];
}

static word e_cpu_decode (const word opcode)
{
	// TODO: Decode the OP CODE for the e_cpu_exec function!
	return opcode;
}

static void e_cpu_exec (e_cpu *a)
{
	word va, vb, vc, vres;
	word op;
	
	for (a->pc = 0; a->pc < E_MAX_FLASH; a->pc++)
	{
		op = e_cpu_decode (a->flash[a->pc]);
		
		// Execute the instruction based on the OP code
		switch (op)
		{
			case CPU_OPC_LDI:
				// Load value <b> into register <a>
				va = e_cpu_get_arg (a);
				vb = e_cpu_get_arg (a);
				
				if (va < CPU_REG_END)
				{
					a->sram[va] = vb;
					e_cpu_update (a, vb);
				}
				break;
			case CPU_OPC_MOV:
				// Move register <b> to register <a> if available
				va = e_cpu_get_arg (a);
				vb = e_cpu_get_arg (a);
				
				if (va < CPU_REG_END &&
				    vb < CPU_REG_END)
				{
					a->sram[va] = a->sram[vb];
				}
				break;
			case CPU_OPC_MOVW:
				// Move word <b> to word <a>
				va = e_cpu_get_arg (a);
				vb = e_cpu_get_arg (a);
				
				if (va < E_MAX_SRAM &&
				    vb < E_MAX_SRAM)
				{
					a->sram[va] = a->sram[vb];
					a->sram[va + 1] = a->sram[vb + 1];
				}
				break;
			case CPU_OPC_PUSH:
				// Add <a> to the stack	
				va = e_cpu_get_arg (a);
				vb = e_cpu_sram_read (a, CPU_REG_RSP);
				
				if (va < CPU_REG_END)
				{
					a->sram[vb--] = a->sram[va];
					e_cpu_sram_write (a, CPU_REG_RSP, vb);
				}
				break;
			case CPU_OPC_POP:
				// Set <a> to the top of the stack	
				va = e_cpu_get_arg (a);
				vb = e_cpu_sram_read (a, CPU_REG_RSP);
				vc = e_cpu_sram_read (a, ++vb);
				
				if (va < CPU_REG_END)
				{
					a->sram[va] = vc;
					e_cpu_sram_write (a, CPU_REG_RSP, vb);
				}
				break;
			case CPU_OPC_ADIW:
				// Add <b> to register <a>
				va = e_cpu_get_arg (a);
				vb = e_cpu_get_arg (a);
				
				if (va < CPU_REG_END)
				{
					vres = a->sram[va] + vb;
					a->sram[va] = vres;
					e_cpu_update (a, vres);
				}
				break;
			case CPU_OPC_ADD:
				// Add register <b> to register <a> 
				va = e_cpu_get_arg (a);
				vb = e_cpu_get_arg (a);
				
				if (va < CPU_REG_END &&
				    vb < CPU_REG_END)
				{
					vres = a->sram[va] + a->sram[vb];
					a->sram[va] = vres;
					e_cpu_update (a, vres);
				}
				break;
			case CPU_OPC_SUBI:
				// Subtract <b> from register <a>
				va = e_cpu_get_arg (a);
				vb = e_cpu_get_arg (a);
				
				if (va < CPU_REG_END)
				{
					vres = a->sram[va] - vb;
					a->sram[va] = vres;
					e_cpu_update (a, vres);
				}
				break;
			case CPU_OPC_SUB:
				// Subtract register <b> from register <a>
				va = e_cpu_get_arg (a);
				vb = e_cpu_get_arg (a);
				
				if (va < CPU_REG_END &&
				    vb < CPU_REG_END)
				{
					vres = a->sram[va] - a->sram[vb];
					a->sram[va] = vres;
					e_cpu_update (a, vres);
				}
				break;
			case CPU_OPC_MUL:
				// Multiply register <a> with register <b>
				va = e_cpu_get_arg (a);
				vb = e_cpu_get_arg (a);
				
				if (va < CPU_REG_END &&
				    vb < CPU_REG_END)
				{
					vres = a->sram[va] * a->sram[vb];
					a->sram[va] = vres;
					e_cpu_update (a, vres);
				}
				break;
			case CPU_OPC_INC:
				// Increment register <a>
				va = e_cpu_get_arg (a);
				
				if (va < CPU_REG_END)
				{
					vres = a->sram[va] + 1;
					a->sram[va] = vres;
					e_cpu_update (a, vres);
				}
				break;
			case CPU_OPC_DEC:
				// Decrement register <a>
				va = e_cpu_get_arg (a);
				
				if (va < CPU_REG_END)
				{
					vres = a->sram[va] - 1;
					a->sram[va] = vres;
					e_cpu_update (a, vres);
				}
				break;
			case CPU_OPC_SWAP:
				// Swap register <a>
				va = e_cpu_get_arg (a);
				
				if (va < CPU_REG_END)
				{
					vb = a->sram[va];
					a->sram[va] = (vb >> 4) | (vb << 4);
				}
				break;
			case CPU_OPC_NEG:
				// Negate register <a>
				va = e_cpu_get_arg (a);
				
				if (va < CPU_REG_END)
				{
					vres = -a->sram[va];
					a->sram[va] = vres;
					e_cpu_update (a, vres);
				}
				break;
			case CPU_OPC_ROR:
				// Rotate register <a>
				va = e_cpu_get_arg (a);
				
				if (va < CPU_REG_END)
				{
					vb = a->sram[va];
					vres = vb >> 1 | (a->sreg[CPU_SREG_N] << 7);
					a->sram[va] = vres;
					e_cpu_update (a, vres);
				}
				break;
			case CPU_OPC_ANDI:
				// Set register <a> to (register <a> & <b>)
				va = e_cpu_get_arg (a);
				vb = e_cpu_get_arg (a);
				
				if (va < CPU_REG_END)
				{
					vres = a->sram[va] & vb;
					a->sram[va] = vres;
					e_cpu_update (a, vres);
				}
				break;
			case CPU_OPC_AND:
				// Set register <a> to (register <a> & register <b>)
				va = e_cpu_get_arg (a);
				vb = e_cpu_get_arg (a);
				
				if (va < CPU_REG_END &&
				    vb < CPU_REG_END)
				{
					vres = a->sram[va] & a->sram[vb];
					a->sram[va] = vres;
					e_cpu_update (a, vres);
				}
				break;
			case CPU_OPC_ORI:
				// Set register <a> to (register <a> | <b>)
				va = e_cpu_get_arg (a);
				vb = e_cpu_get_arg (a);
				
				if (va < CPU_REG_END)
				{
					vres = a->sram[va] | vb;
					a->sram[va] = vres;
					e_cpu_update (a, vres);
				}
				break;
			case CPU_OPC_OR:
				// Set register <a> to (register <a> | register <b>)
				va = e_cpu_get_arg (a);
				vb = e_cpu_get_arg (a);
				
				if (va < CPU_REG_END &&
				    vb < CPU_REG_END)
				{
					vres = a->sram[va] | a->sram[vb];
					a->sram[va] = vres;
					e_cpu_update (a, vres);
				}
				break;
			case CPU_OPC_EOR:
				// Set register <a> to (register <a> ^ register <b>)
				va = e_cpu_get_arg (a);
				vb = e_cpu_get_arg (a);
				
				if (va < CPU_REG_END &&
				    vb < CPU_REG_END)
				{
					vres = a->sram[va] ^ a->sram[vb];
					a->sram[va] = vres;
					e_cpu_update (a, vres);
				}
				break;
			case CPU_OPC_CPI:
				// Set the ZERO flag to (register <a> == <b>)
				va = e_cpu_get_arg (a);
				vb = e_cpu_get_arg (a);
				
				if (va < CPU_REG_END)
				{
					vres = a->sram[va] - vb;
					e_cpu_update (a, vres);
				}
				break;
			case CPU_OPC_CP:
				// Set the ZERO flag to (register <a> == register <b>)
				va = e_cpu_get_arg (a);
				vb = e_cpu_get_arg (a);
				
				if (va < CPU_REG_END &&
				    vb < CPU_REG_END)
				{
					vres = a->sram[va] - a->sram[vb];
					e_cpu_update (a, vres);
				}
				break;
			case CPU_OPC_JZ:
			case CPU_OPC_JE:
				// Set the instruction pointer to <a> if its valid and ZERO
				va = e_cpu_get_arg (a);
				
				if (a->sreg[CPU_SREG_Z])
				{
					a->pc = va;
				}
				break;
			case CPU_OPC_JNE:
				// Set the instruction pointer to <a> if its valid and NOT ZERO
				va = e_cpu_get_arg (a);
				
				if (!a->sreg[CPU_SREG_Z])
				{
					a->pc = va;
				}
				break;
			case CPU_OPC_BREAK:
				// TODO: This should be implemented to use break points!
				// Quit program
				a->pc = E_MAX_FLASH;
				break;
			case CPU_OPC_NOP:
				// Do nothing
				break;
			case CPU_OPC_OUT:
				// Print register <a>
				va = e_cpu_get_arg (a);
				vb = e_cpu_get_arg (a);
				
				if (va < E_MAX_SRAM &&
				    vb < CPU_REG_END)
				{
					vres = a->sram[vb];
					a->sram[va] = vres;
					
					// Output to console
					if (va == E_PORT_STDIO)
					{
						printf ("%c", vres);
					}
					else if (va == E_PORT_EXIT)
					{
						// Quit program
						a->pc = E_MAX_FLASH;
					}
				}
				break;
			case CPU_OPC_IN:
				// Input -> register <b> into register <a>
				va = e_cpu_get_arg (a);
				vb = e_cpu_get_arg (a);
				
				if (va < CPU_REG_END &&
				    vb < E_MAX_SRAM)
				{
					a->sram[va] = a->sram[vb];
				}
				break;
			case CPU_OPC_BAD:
				// Bad instruction
				#ifdef __DEBUG__
				printf ("Bad instruction at pc=%u\n", a->pc);
				#endif
				break;
			default:
				// Unknown OP code
				#ifdef __DEBUG__
				printf ("UNKNOWN OP: %u\n", (unsigned int) a->flash[a->pc]);
				#endif
				break;
		}
	}
}

// Run a program
static void e_run (const word *prg)
{
	e_cpu cpu;
	
	// INITIALIZATION
	e_cpu_init (&cpu, prg);
	
	// RUNTIME
	e_cpu_exec (&cpu);
	
	// CLEANING
	e_cpu_free (&cpu);
}

//
// TEST PROGRAMS
//
// Counter
static void e_tst_counter (void)
{
	word prg[59];
	
	// CODE Explained:
	// LOOP A values	
	prg[0] = CPU_OPC_LDI;
	prg[1] = CPU_REG_R8;
	prg[2] = 20;
	
	prg[3] = CPU_OPC_LDI;
	prg[4] = CPU_REG_R9;
	prg[5] = 130;
	
	// LOOP B values
	prg[6] = CPU_OPC_LDI;
	prg[7] = CPU_REG_R10;
	prg[8] = 255;
	
	prg[9] = CPU_OPC_LDI;
	prg[10] = CPU_REG_R11;
	prg[11] = 255;
	
	prg[12] = CPU_OPC_LDI;
	prg[13] = CPU_REG_R12;
	prg[14] = 255;
	
	// LOOP A
	prg[15] = CPU_OPC_DEC;
	prg[16] = CPU_REG_R9;
	
	prg[17] = CPU_OPC_OUT;
	prg[18] = E_PORT_STDIO;
	prg[19] = CPU_REG_R9;
	
	// LOOP B-1
	prg[20] = CPU_OPC_DEC;
	prg[21] = CPU_REG_R10;
	
	// LOOP B-2
	prg[22] = CPU_OPC_DEC;
	prg[23] = CPU_REG_R11;
	
	// LOOP B-3
	prg[24] = CPU_OPC_DEC;
	prg[25] = CPU_REG_R12;
	
	// LOOP B-3 END
	// skip if R5 != R1(0)
	prg[26] = CPU_OPC_CP;
	prg[27] = CPU_REG_R12;
	prg[28] = CPU_REG_R8;
	prg[29] = CPU_OPC_JNE;
	prg[30] = 23;
	// RESET
	prg[31] = CPU_OPC_LDI;
	prg[32] = CPU_REG_R12;
	prg[33] = 255;
	
	// LOOP B-2 END
	// skip if R4 != R1(0)
	prg[34] = CPU_OPC_CP;
	prg[35] = CPU_REG_R11;
	prg[36] = CPU_REG_R8;
	prg[37] = CPU_OPC_JNE;
	prg[38] = 21;
	// RESET
	prg[39] = CPU_OPC_LDI;
	prg[40] = CPU_REG_R11;
	prg[41] = 255;
	
	// LOOP B-1 END
	// skip if R3 != R1(0)
	prg[42] = CPU_OPC_CP;
	prg[43] = CPU_REG_R10;
	prg[44] = CPU_REG_R8;
	prg[45] = CPU_OPC_JNE;
	prg[46] = 19;
	// RESET
	prg[47] = CPU_OPC_LDI;
	prg[48] = CPU_REG_R10;
	prg[49] = 255;
	
	// LOOP A END
	// skip if R2 != R1(0)
	prg[50] = CPU_OPC_CP;
	prg[51] = CPU_REG_R9;
	prg[52] = CPU_REG_R8;
	prg[53] = CPU_OPC_JNE;
	prg[54] = 14;
	
	// BREAK
	prg[55] = CPU_OPC_OUT;
	prg[56] = E_PORT_EXIT;
	prg[57] = CPU_REG_R8;
	
	prg[58] = '\0';
	
	e_run (prg);
}

static void e_tst_tst (void)
{
	word prg[15];
	
	prg[0] = CPU_OPC_LDI;
	prg[1] = CPU_REG_R8;
	prg[2] = 128;
	
	prg[3] = CPU_OPC_OUT;
	prg[4] = E_PORT_STDIO;
	prg[5] = CPU_REG_R8;
	
	prg[6] = CPU_OPC_ROR;
	prg[7] = CPU_REG_R8;
	
	prg[8] = CPU_OPC_OUT;
	prg[9] = E_PORT_STDIO;
	prg[10] = CPU_REG_R8;
	
	prg[11] = CPU_OPC_OUT;
	prg[12] = E_PORT_EXIT;
	prg[13] = CPU_REG_R8;
	
	prg[14] = '\0';
	
	e_run (prg);
}

int main (int argc, char *argv[])
{
	printf ("Emulator v. 0.0.1 A (C) 2016 Marc Volker Dickmann\n\n");
	
	e_tst_counter ();
	//e_tst_tst ();
	
	return 0;
}
