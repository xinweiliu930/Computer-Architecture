/*
    Name 1: Xinwei Liu
    UTEID 1: xl7847
*/

/***************************************************************/
/*                                                             */
/*   LC-3b Simulator                                           */
/*                                                             */
/*   EE 460N - Lab 5                                           */
/*   The University of Texas at Austin                         */
/*                                                             */
/***************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/***************************************************************/
/*                                                             */
/* Files:  ucode        Microprogram file                      */
/*         pagetable    page table in LC-3b machine language   */
/*         isaprogram   LC-3b machine language program file    */
/*                                                             */
/***************************************************************/

/***************************************************************/
/* These are the functions you'll have to write.               */
/***************************************************************/

void eval_micro_sequencer();
void cycle_memory();
void eval_bus_drivers();
void drive_bus();
void latch_datapath_values();

/***************************************************************/
/* A couple of useful definitions.                             */
/***************************************************************/
#define FALSE 0
#define TRUE  1

/***************************************************************/
/* Use this to avoid overflowing 16 bits on the bus.           */
/***************************************************************/
#define Low16bits(x) ((x) & 0xFFFF)

/***************************************************************/
/* Definition of the control store layout.                     */
/***************************************************************/
#define CONTROL_STORE_ROWS 64
#define INITIAL_STATE_NUMBER 18

/***************************************************************/
/* Definition of bit order in control store word.              */
/***************************************************************/
enum CS_BITS {                                                  
    IRD,
    COND1, COND0,
    J5, J4, J3, J2, J1, J0,
    LD_MAR,
    LD_MDR,
    LD_IR,
    LD_BEN,
    LD_REG,
    LD_CC,
    LD_PC,
    GATE_PC,
    GATE_MDR,
    GATE_ALU,
    GATE_MARMUX,
    GATE_SHF,
    PCMUX1, PCMUX0,
    DRMUX1, DRMUX0,
    SR1MUX1,SR1MUX0, 
    ADDR1MUX,
    ADDR2MUX1, ADDR2MUX0,
    MARMUX,
    ALUK1, ALUK0,
    MIO_EN,
    R_W,
    DATA_SIZE,
    LSHF1,
	
	LD_USP,
    LD_SSP,
    SPMUX1,
    SPMUX0,
    Gate_SP,
    LD_PSR,
    Gate_PSR,
    LD_PSR15,
    MODEMUX,
    LD_EXCV,
    VECTMUX,
    LD_VECT,
    Gate_VECT,
    INTMUX,
    LD_INT,
    LD_EXP,
    EXPMUX,
    GatePC1,
    LD_PSRNZP,
	
	LDSTMUX3,LDSTMUX2,LDSTMUX1,LDSTMUX0,
	LD_LDST,
	GatePTBR,
	LD_VA,
	GateVA,
	LD_PTE,
	GatePTE,
	PRPG,
	GateMAR,
	IRM,
	LD_VPN,
	LD_PFN,
	LD_PTER,
	LD_PTEM,
	LD_MD,
	MDMUX,
	WORDALIGN,
	PROTECTION,
	LD_PSRSAVED,
	GatePSRSAVED,
/* MODIFY: you have to add all your new control signals */
    CONTROL_STORE_BITS
} CS_BITS;

/***************************************************************/
/* Functions to get at the control bits.                       */
/***************************************************************/
int GetIRD(int *x)           { return(x[IRD]); }
int GetCOND(int *x)          { return((x[COND1] << 1) + x[COND0]); }
int GetJ(int *x)             { return((x[J5] << 5) + (x[J4] << 4) +
				      (x[J3] << 3) + (x[J2] << 2) +
				      (x[J1] << 1) + x[J0]); }
int GetLD_MAR(int *x)        { return(x[LD_MAR]); }
int GetLD_MDR(int *x)        { return(x[LD_MDR]); }
int GetLD_IR(int *x)         { return(x[LD_IR]); }
int GetLD_BEN(int *x)        { return(x[LD_BEN]); }
int GetLD_REG(int *x)        { return(x[LD_REG]); }
int GetLD_CC(int *x)         { return(x[LD_CC]); }
int GetLD_PC(int *x)         { return(x[LD_PC]); }
int GetGATE_PC(int *x)       { return(x[GATE_PC]); }
int GetGATE_MDR(int *x)      { return(x[GATE_MDR]); }
int GetGATE_ALU(int *x)      { return(x[GATE_ALU]); }
int GetGATE_MARMUX(int *x)   { return(x[GATE_MARMUX]); }
int GetGATE_SHF(int *x)      { return(x[GATE_SHF]); }
int GetPCMUX(int *x)         { return((x[PCMUX1] << 1) + x[PCMUX0]); }
int GetDRMUX(int *x)         { return((x[DRMUX1] << 1) + x[DRMUX0]); }
int GetSR1MUX(int *x)        { return((x[SR1MUX1]<< 1) + x[SR1MUX0]); }
int GetADDR1MUX(int *x)      { return(x[ADDR1MUX]); }
int GetADDR2MUX(int *x)      { return((x[ADDR2MUX1] << 1) + x[ADDR2MUX0]); }
int GetMARMUX(int *x)        { return(x[MARMUX]); }
int GetALUK(int *x)          { return((x[ALUK1] << 1) + x[ALUK0]); }
int GetMIO_EN(int *x)        { return(x[MIO_EN]); }
int GetR_W(int *x)           { return(x[R_W]); }
int GetDATA_SIZE(int *x)     { return(x[DATA_SIZE]); } 
int GetLSHF1(int *x)         { return(x[LSHF1]); }

int GetLD_USP(int *x)         { return(x[LD_USP]); }
int GetLD_SSP(int *x)         { return(x[LD_SSP]); }
int GetSPMUX(int *x)         { return((x[SPMUX1] << 1) + x[SPMUX0]); }
int GetGate_SP(int *x)         { return(x[Gate_SP]); }
int GetLD_PSR(int *x)         { return(x[LD_PSR]); }
int GetGate_PSR(int *x)         { return(x[Gate_PSR]); }
int GetLD_PSR15(int *x)         { return(x[LD_PSR15]); }
int GetMODEMUX(int *x)         { return(x[MODEMUX]); }
int GetLD_EXCV(int *x)         { return(x[LD_EXCV]); }
int GetVECTMUX(int *x)         { return(x[VECTMUX]); }
int GetLD_VECT(int *x)         { return(x[LD_VECT]); }
int GetGate_VECT(int *x)         { return(x[Gate_VECT]); }
int GetINTMUX(int *x)         { return(x[INTMUX]); }
int GetLD_INT(int *x)         { return(x[LD_INT]); }
int GetLD_EXP(int *x)         { return(x[LD_EXP]); }
int GetEXPMUX(int *x)         { return(x[EXPMUX]); }
int GetGatePC1(int *x)         { return(x[GatePC1]); }
int GetLD_PSRNZP(int *x)         { return(x[LD_PSRNZP]); }

int GetLDSTMUX(int *x)         { return((x[LDSTMUX3]<<3) + (x[LDSTMUX2]<<2) + (x[LDSTMUX1]<<1) + x[LDSTMUX0]); }
int GetLD_LDST(int *x)         { return(x[LD_LDST]); }
int GetGatePTBR(int *x)         { return(x[GatePTBR]); }
int GetLD_VA(int *x)         { return(x[LD_VA]); }
int GetGateVA(int *x)         { return(x[GateVA]); }
int GetLD_PTE(int *x)         { return(x[LD_PTE]); }
int GetGatePTE(int *x)         { return(x[GatePTE]); }
int GetPRPG(int *x)         { return(x[PRPG]); }
int GetGateMAR(int *x)         { return(x[GateMAR]); }
int GetIRM(int *x)         { return(x[IRM]); }
int GetLD_VPN(int *x)         { return(x[LD_VPN]); }
int GetLD_PFN(int *x)         { return(x[LD_PFN]); }
int GetLD_PTER(int *x)         { return(x[LD_PTER]); }
int GetLD_PTEM(int *x)         { return(x[LD_PTEM]); }
int GetLD_MD(int *x)         { return(x[LD_MD]); }
int GetMDMUX(int *x)         { return(x[MDMUX]); }
int GetWORDALIGN(int *x)        { return(x[WORDALIGN]); }
int GetPROTECTION(int *x)         { return(x[PROTECTION]); }
int GetPSRSAVED(int *x)         { return(x[LD_PSRSAVED]); }
int GetGatePSRSAVED(int *x)         { return(x[GatePSRSAVED]); }
/* MODIFY: you can add more Get functions for your new control signals */

/***************************************************************/
/* The control store rom.                                      */
/***************************************************************/
int CONTROL_STORE[CONTROL_STORE_ROWS][CONTROL_STORE_BITS];

/***************************************************************/
/* Main memory.                                                */
/***************************************************************/
/* MEMORY[A][0] stores the least significant byte of word at word address A
   MEMORY[A][1] stores the most significant byte of word at word address A 
   There are two write enable signals, one for each byte. WE0 is used for 
   the least significant byte of a word. WE1 is used for the most significant 
   byte of a word. */

#define WORDS_IN_MEM    0x2000 /* 32 frames */ 
#define MEM_CYCLES      5
int MEMORY[WORDS_IN_MEM][2];

/***************************************************************/

/***************************************************************/

/***************************************************************/
/* LC-3b State info.                                           */
/***************************************************************/
#define LC_3b_REGS 8

int RUN_BIT;	/* run bit */
int BUS;	/* value of the bus */

typedef struct System_Latches_Struct{

int PC,		/* program counter */
    MDR,	/* memory data register */
    MAR,	/* memory address register */
    IR,		/* instruction register */
    N,		/* n condition bit */
    Z,		/* z condition bit */
    P,		/* p condition bit */
    BEN;        /* ben register */

int READY;	/* ready bit */
  /* The ready bit is also latched as you dont want the memory system to assert it 
     at a bad point in the cycle*/

int REGS[LC_3b_REGS]; /* register file. */

int MICROINSTRUCTION[CONTROL_STORE_BITS]; /* The microintruction */

int STATE_NUMBER; /* Current State Number - Provided for debugging */ 

/* For lab 4 */
int INTV; /* Interrupt vector register */
int EXCV; /* Exception vector register */
int SSP; /* Initial value of system stack pointer */
/* MODIFY: you should add here any other registers you need to implement interrupts and exceptions */
int USP, PSR, VECT, EXP, INT;
/* For lab 5 */
int PTBR; /* This is initialized when we load the page table */
int VA;   /* Temporary VA register */
/* MODIFY: you should add here any other registers you need to implement virtual memory */
int PTE, LDST, MD,PSRSAVED;
} System_Latches;

/* Data Structure for Latch */

System_Latches CURRENT_LATCHES, NEXT_LATCHES;

/* For lab 5 */
#define PAGE_NUM_BITS 9
#define PTE_PFN_MASK 0x3E00
#define PTE_VALID_MASK 0x0004
#define PAGE_OFFSET_MASK 0x1FF

/***************************************************************/
/* A cycle counter.                                            */
/***************************************************************/
int CYCLE_COUNT;

/***************************************************************/
/*                                                             */
/* Procedure : help                                            */
/*                                                             */
/* Purpose   : Print out a list of commands.                   */
/*                                                             */
/***************************************************************/
void help() {                                                    
    printf("----------------LC-3bSIM Help-------------------------\n");
    printf("go               -  run program to completion       \n");
    printf("run n            -  execute program for n cycles    \n");
    printf("mdump low high   -  dump memory from low to high    \n");
    printf("rdump            -  dump the register & bus values  \n");
    printf("?                -  display this help menu          \n");
    printf("quit             -  exit the program                \n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : cycle                                           */
/*                                                             */
/* Purpose   : Execute a cycle                                 */
/*                                                             */
/***************************************************************/
void cycle() {                                                

  eval_micro_sequencer();   
  cycle_memory();
  eval_bus_drivers();
  drive_bus();
  latch_datapath_values();

  CURRENT_LATCHES = NEXT_LATCHES;

  CYCLE_COUNT++;
}

/***************************************************************/
/*                                                             */
/* Procedure : run n                                           */
/*                                                             */
/* Purpose   : Simulate the LC-3b for n cycles.                 */
/*                                                             */
/***************************************************************/
void run(int num_cycles) {                                      
    int i;

    if (RUN_BIT == FALSE) {
	printf("Can't simulate, Simulator is halted\n\n");
	return;
    }

    printf("Simulating for %d cycles...\n\n", num_cycles);
    for (i = 0; i < num_cycles; i++) {
	if (CURRENT_LATCHES.PC == 0x0000) {
	    RUN_BIT = FALSE;
	    printf("Simulator halted\n\n");
	    break;
	}
	cycle();
    }
}

/***************************************************************/
/*                                                             */
/* Procedure : go                                              */
/*                                                             */
/* Purpose   : Simulate the LC-3b until HALTed.                 */
/*                                                             */
/***************************************************************/
void go() {                                                     
    if (RUN_BIT == FALSE) {
	printf("Can't simulate, Simulator is halted\n\n");
	return;
    }

    printf("Simulating...\n\n");
    while (CURRENT_LATCHES.PC != 0x0000)
	cycle();
    RUN_BIT = FALSE;
    printf("Simulator halted\n\n");
}

/***************************************************************/ 
/*                                                             */
/* Procedure : mdump                                           */
/*                                                             */
/* Purpose   : Dump a word-aligned region of memory to the     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void mdump(FILE * dumpsim_file, int start, int stop) {          
    int address; /* this is a byte address */

    printf("\nMemory content [0x%0.4x..0x%0.4x] :\n", start, stop);
    printf("-------------------------------------\n");
    for (address = (start >> 1); address <= (stop >> 1); address++)
	printf("  0x%0.4x (%d) : 0x%0.2x%0.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
    printf("\n");

    /* dump the memory contents into the dumpsim file */
    fprintf(dumpsim_file, "\nMemory content [0x%0.4x..0x%0.4x] :\n", start, stop);
    fprintf(dumpsim_file, "-------------------------------------\n");
    for (address = (start >> 1); address <= (stop >> 1); address++)
	fprintf(dumpsim_file, " 0x%0.4x (%d) : 0x%0.2x%0.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
    fprintf(dumpsim_file, "\n");
    fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : rdump                                           */
/*                                                             */
/* Purpose   : Dump current register and bus values to the     */   
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void rdump(FILE * dumpsim_file) {                               
    int k; 

    printf("\nCurrent register/bus values :\n");
    printf("-------------------------------------\n");
    printf("Cycle Count  : %d\n", CYCLE_COUNT);
    printf("PC           : 0x%0.4x\n", CURRENT_LATCHES.PC);
    printf("IR           : 0x%0.4x\n", CURRENT_LATCHES.IR);
    printf("STATE_NUMBER : 0x%0.4x\n\n", CURRENT_LATCHES.STATE_NUMBER);
    printf("BUS          : 0x%0.4x\n", BUS);
    printf("MDR          : 0x%0.4x\n", CURRENT_LATCHES.MDR);
    printf("MAR          : 0x%0.4x\n", CURRENT_LATCHES.MAR);
    printf("CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
	printf("PTE          : 0x%0.4x\n", CURRENT_LATCHES.PTE);
	printf("EXP          : 0x%0.4x\n", CURRENT_LATCHES.EXP);
	printf("EXCV         : 0x%0.4x\n", CURRENT_LATCHES.EXCV);
	printf("PSR          : 0x%0.4x\n", CURRENT_LATCHES.PSR);
    printf("Registers:\n");
    for (k = 0; k < LC_3b_REGS; k++)
	printf("%d: 0x%0.4x\n", k, CURRENT_LATCHES.REGS[k]);
    printf("\n");

    /* dump the state information into the dumpsim file */
    fprintf(dumpsim_file, "\nCurrent register/bus values :\n");
    fprintf(dumpsim_file, "-------------------------------------\n");
    fprintf(dumpsim_file, "Cycle Count  : %d\n", CYCLE_COUNT);
    fprintf(dumpsim_file, "PC           : 0x%0.4x\n", CURRENT_LATCHES.PC);
    fprintf(dumpsim_file, "IR           : 0x%0.4x\n", CURRENT_LATCHES.IR);
    fprintf(dumpsim_file, "STATE_NUMBER : 0x%0.4x\n\n", CURRENT_LATCHES.STATE_NUMBER);
    fprintf(dumpsim_file, "BUS          : 0x%0.4x\n", BUS);
    fprintf(dumpsim_file, "MDR          : 0x%0.4x\n", CURRENT_LATCHES.MDR);
    fprintf(dumpsim_file, "MAR          : 0x%0.4x\n", CURRENT_LATCHES.MAR);
    fprintf(dumpsim_file, "CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
    fprintf(dumpsim_file, "Registers:\n");
    for (k = 0; k < LC_3b_REGS; k++)
	fprintf(dumpsim_file, "%d: 0x%0.4x\n", k, CURRENT_LATCHES.REGS[k]);
    fprintf(dumpsim_file, "\n");
    fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : get_command                                     */
/*                                                             */
/* Purpose   : Read a command from standard input.             */  
/*                                                             */
/***************************************************************/
void get_command(FILE * dumpsim_file) {                         
    char buffer[20];
    int start, stop, cycles;

    printf("LC-3b-SIM> ");

    scanf("%s", buffer);
    printf("\n");

    switch(buffer[0]) {
    case 'G':
    case 'g':
	go();
	break;

    case 'M':
    case 'm':
	scanf("%i %i", &start, &stop);
	mdump(dumpsim_file, start, stop);
	break;

    case '?':
	help();
	break;
    case 'Q':
    case 'q':
	printf("Bye.\n");
	exit(0);

    case 'R':
    case 'r':
	if (buffer[1] == 'd' || buffer[1] == 'D')
	    rdump(dumpsim_file);
	else {
	    scanf("%d", &cycles);
	    run(cycles);
	}
	break;

    default:
	printf("Invalid Command\n");
	break;
    }
}

/***************************************************************/
/*                                                             */
/* Procedure : init_control_store                              */
/*                                                             */
/* Purpose   : Load microprogram into control store ROM        */ 
/*                                                             */
/***************************************************************/
void init_control_store(char *ucode_filename) {                 
    FILE *ucode;
    int i, j, index;
    char line[200];

    printf("Loading Control Store from file: %s\n", ucode_filename);

    /* Open the micro-code file. */
    if ((ucode = fopen(ucode_filename, "r")) == NULL) {
	printf("Error: Can't open micro-code file %s\n", ucode_filename);
	exit(-1);
    }

    /* Read a line for each row in the control store. */
    for(i = 0; i < CONTROL_STORE_ROWS; i++) {
	if (fscanf(ucode, "%[^\n]\n", line) == EOF) {
	    printf("Error: Too few lines (%d) in micro-code file: %s\n",
		   i, ucode_filename);
	    exit(-1);
	}

	/* Put in bits one at a time. */
	index = 0;

	for (j = 0; j < CONTROL_STORE_BITS; j++) {
	    /* Needs to find enough bits in line. */
	    if (line[index] == '\0') {
		printf("Error: Too few control bits in micro-code file: %s\nLine: %d\n",
		       ucode_filename, i);
		exit(-1);
	    }
	    if (line[index] != '0' && line[index] != '1') {
		printf("Error: Unknown value in micro-code file: %s\nLine: %d, Bit: %d\n",
		       ucode_filename, i, j);
		exit(-1);
	    }

	    /* Set the bit in the Control Store. */
	    CONTROL_STORE[i][j] = (line[index] == '0') ? 0:1;
	    index++;
	}

	/* Warn about extra bits in line. */
	if (line[index] != '\0')
	    printf("Warning: Extra bit(s) in control store file %s. Line: %d\n",
		   ucode_filename, i);
    }
    printf("\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : init_memory                                     */
/*                                                             */
/* Purpose   : Zero out the memory array                       */
/*                                                             */
/***************************************************************/
void init_memory() {                                           
    int i;

    for (i=0; i < WORDS_IN_MEM; i++) {
	MEMORY[i][0] = 0;
	MEMORY[i][1] = 0;
    }
}

/**************************************************************/
/*                                                            */
/* Procedure : load_program                                   */
/*                                                            */
/* Purpose   : Load program and service routines into mem.    */
/*                                                            */
/**************************************************************/
void load_program(char *program_filename, int is_virtual_base) {                   
    FILE * prog;
    int ii, word, program_base, pte, virtual_pc;

    /* Open program file. */
    prog = fopen(program_filename, "r");
    if (prog == NULL) {
	printf("Error: Can't open program file %s\n", program_filename);
	exit(-1);
    }

    /* Read in the program. */
    if (fscanf(prog, "%x\n", &word) != EOF)
	program_base = word >> 1;
    else {
	printf("Error: Program file is empty\n");
	exit(-1);
    }

    if (is_virtual_base) {
      if (CURRENT_LATCHES.PTBR == 0) {
	printf("Error: Page table base not loaded %s\n", program_filename);
	exit(-1);
      }

      /* convert virtual_base to physical_base */
      virtual_pc = program_base << 1;
      pte = (MEMORY[(CURRENT_LATCHES.PTBR + (((program_base << 1) >> PAGE_NUM_BITS) << 1)) >> 1][1] << 8) | 
	     MEMORY[(CURRENT_LATCHES.PTBR + (((program_base << 1) >> PAGE_NUM_BITS) << 1)) >> 1][0];

      printf("virtual base of program: %04x\npte: %04x\n", program_base << 1, pte);
		if ((pte & PTE_VALID_MASK) == PTE_VALID_MASK) {
	      program_base = (pte & PTE_PFN_MASK) | ((program_base << 1) & PAGE_OFFSET_MASK);
   	   printf("physical base of program: %x\n\n", program_base);
	      program_base = program_base >> 1; 
		} else {
   	   printf("attempting to load a program into an invalid (non-resident) page\n\n");
			exit(-1);
		}
    }
    else {
      /* is page table */
     CURRENT_LATCHES.PTBR = program_base << 1;
    }

    ii = 0;
    while (fscanf(prog, "%x\n", &word) != EOF) {
	/* Make sure it fits. */
	if (program_base + ii >= WORDS_IN_MEM) {
	    printf("Error: Program file %s is too long to fit in memory. %x\n",
		   program_filename, ii);
	    exit(-1);
	}

	/* Write the word to memory array. */
	MEMORY[program_base + ii][0] = word & 0x00FF;
	MEMORY[program_base + ii][1] = (word >> 8) & 0x00FF;;
	ii++;
    }

    if (CURRENT_LATCHES.PC == 0 && is_virtual_base) 
      CURRENT_LATCHES.PC = virtual_pc;

    printf("Read %d words from program into memory.\n\n", ii);
}

/***************************************************************/
/*                                                             */
/* Procedure : initialize                                      */
/*                                                             */
/* Purpose   : Load microprogram and machine language program  */ 
/*             and set up initial state of the machine         */
/*                                                             */
/***************************************************************/
void initialize(char *argv[], int num_prog_files) { 
    int i;
    init_control_store(argv[1]);

    init_memory();
    load_program(argv[2],0);
    for ( i = 0; i < num_prog_files; i++ ) {
	load_program(argv[i + 3],1);
    }
    CURRENT_LATCHES.Z = 1;
    CURRENT_LATCHES.STATE_NUMBER = INITIAL_STATE_NUMBER;
    memcpy(CURRENT_LATCHES.MICROINSTRUCTION, CONTROL_STORE[INITIAL_STATE_NUMBER], sizeof(int)*CONTROL_STORE_BITS);
    CURRENT_LATCHES.SSP = 0x3000; /* Initial value of system stack pointer */

/* MODIFY: you can add more initialization code HERE */
    CURRENT_LATCHES.PSR = 0x8002;
    CURRENT_LATCHES.REGS[6] = 0xFE00;
    CURRENT_LATCHES.INT = 0;
	CURRENT_LATCHES.EXP = 0;

    NEXT_LATCHES = CURRENT_LATCHES;

    RUN_BIT = TRUE;
}

/***************************************************************/
/*                                                             */
/* Procedure : main                                            */
/*                                                             */
/***************************************************************/
int main(int argc, char *argv[]) {                              
    FILE * dumpsim_file;

    /* Error Checking */
    if (argc < 4) {
	printf("Error: usage: %s <micro_code_file> <page table file> <program_file_1> <program_file_2> ...\n",
	       argv[0]);
	exit(1);
    }

    printf("LC-3b Simulator\n\n");

    initialize(argv, argc - 3);

    if ( (dumpsim_file = fopen( "dumpsim", "w" )) == NULL ) {
	printf("Error: Can't open dumpsim file\n");
	exit(-1);
    }

    while (1)
	get_command(dumpsim_file);

}

/***************************************************************/
/* Do not modify the above code, except for the places indicated 
   with a "MODIFY:" comment.
   You are allowed to use the following global variables in your
   code. These are defined above.

   CONTROL_STORE
   MEMORY
   BUS

   CURRENT_LATCHES
   NEXT_LATCHES

   You may define your own local/global variables and functions.
   You may use the functions to get at the control bits defined
   above.

   Begin your code here 	  			       */
/***************************************************************/


int rc = 0,temp=0,MARMUX2=0,ALU2 = 0, SHF2 = 0, PC2 = 0, MDR2 = 0,ADDER1=0,ADDER2=0,high=0, low=0, clkct = 0, SP2 = 0, excv2 = 0, pte2 = 0, va2 = 0;

int getdr(int a)
{  int b=0;
   a = a&0x0E;
   b = a >> 1;
   return b;
}

int getsr1(int a,int b)
{
   int c=0;
   a = a&0x01;
   a = a << 2;
   b = b&0xC0;
   b = b >> 6;
   c = a|b;
   return c;
}

int sext5(int a)
{ int b,c,d;
  b = a&0x10;
  c = a&0x20;
  if (b!=c)
  d = a|0xFFE0;
  else d = a;
  return d;
}

int sext6(int a)
{ int b,c,d;
  b = a&0x20;
  c = a&0x40;
  if (b!=c)
  d = a|0xFFC0;
  else d = a;
  return d;
}

int sext8(int a)
{ int b,c,d;
  b = a&0x80;
  c = a&0x100;
  if (b!=c)
  d = a|0xFF00;
  else d = a;
  return d;
}
 
int sext9(int a)
{ int b,c,d;
  b = a&0x100;
  c = a&0x200;
  if (b!=c)
  d = a|0xFE00;
  else d = a;
  return d;
}

int sext11(int a)
{ int b,c,d;
  b = a&0x400;
  c = a&0x800;
  if (b!=c)
  d = a|0xF800;
  else d = a;
  return d;
}  




void eval_micro_sequencer() {
clkct = clkct + 1;
if (clkct == 299)
{
  NEXT_LATCHES.INT = 1;
  NEXT_LATCHES.INTV = 0x1;
}
    

  /* 
   * Evaluate the address of the next state according to the 
   * micro sequencer logic. Latch the next microinstruction.
   */
int JB = GetJ(CURRENT_LATCHES.MICROINSTRUCTION);
int IRD2 = GetIRD(CURRENT_LATCHES.MICROINSTRUCTION);
int COND2 = GetCOND(CURRENT_LATCHES.MICROINSTRUCTION);
int INT2 = GetLD_INT(CURRENT_LATCHES.MICROINSTRUCTION);
     //printf("%x\n",CURRENT_LATCHES.EXP);
 if (GetIRM(CURRENT_LATCHES.MICROINSTRUCTION) == 0)
 {	
     if (CURRENT_LATCHES.EXP!=0) 
        NEXT_LATCHES.STATE_NUMBER = 10;
     else if (IRD2 == 1)
     {
       NEXT_LATCHES.STATE_NUMBER = (CURRENT_LATCHES.IR>>12)&0xF; 
     }
     else 
     {
       if(COND2 == 0)
       {
         if ((INT2 == 1) && (CURRENT_LATCHES.INT == 1))
          {
            NEXT_LATCHES.STATE_NUMBER = JB + 8;
          }
         else
          {
            NEXT_LATCHES.STATE_NUMBER = JB;
          }
       }
       else if (COND2 == 1)
       {
         if(CURRENT_LATCHES.READY == 1 )
         {
           NEXT_LATCHES.STATE_NUMBER = JB + 2; //printf("%x\n",JB+2);
         }
         else 
         {
           NEXT_LATCHES.STATE_NUMBER = JB;
         }
       }
       else if (COND2 == 2)
       {
          if ((CURRENT_LATCHES.BEN == 1) && (CURRENT_LATCHES.MICROINSTRUCTION [J2]==0))
          {
            NEXT_LATCHES.STATE_NUMBER = JB + 4;
          }
          else 
          {
            NEXT_LATCHES.STATE_NUMBER = JB;
          }
        }
        else if (COND2 == 3)
        {
           if (((CURRENT_LATCHES.IR&0x800)!= 0) && (CURRENT_LATCHES.MICROINSTRUCTION [J0]==0))
           {
             NEXT_LATCHES.STATE_NUMBER = JB + 1;
           }
           else
           { 
             NEXT_LATCHES.STATE_NUMBER = JB;
           }
         }
      
      }
 }
 else 
 {
	 NEXT_LATCHES.STATE_NUMBER = CURRENT_LATCHES.LDST;
 }
 memcpy(NEXT_LATCHES.MICROINSTRUCTION, CONTROL_STORE[NEXT_LATCHES.STATE_NUMBER], sizeof(int)*CONTROL_STORE_BITS);
}


void cycle_memory() {
 
  /* 
   * This function emulates memory and the WE logic. 
   * Keep track of which cycle of MEMEN we are dealing with.  
   * If fourth, we need to latch Ready bit at the end of 
   * cycle to prepare microsequencer for the fifth cycle.  
   */
//printf("%x\n",rc);
if ((GetMIO_EN(CURRENT_LATCHES.MICROINSTRUCTION) == 1) && (rc<5))
{
      rc = rc+1;
    if (rc == 4)
    {
      NEXT_LATCHES.READY = 1;//printf("hello");
    }

}

if ((GetMIO_EN(CURRENT_LATCHES.MICROINSTRUCTION) == 1) && (rc == 5))
{
      rc = 0;
      temp = (MEMORY[CURRENT_LATCHES.MAR >> 1][0]|(MEMORY[CURRENT_LATCHES.MAR >> 1][1]<<8))&0xFFFF;//printf("%x\n",temp);
      NEXT_LATCHES.READY = 0;
}




}



void eval_bus_drivers() {

  /* 
   * Datapath routine emulating operations before driving the bus.
   * Evaluate the input of tristate drivers 
   *             Gate_MARMUX,
   *		 Gate_PC,
   *		 Gate_ALU,
   *		 Gate_SHF,
   *		 Gate_MDR.
   */    
int ADDER3=0,ADDER4=0,IMM=0;
         high = (CURRENT_LATCHES.IR & 0xFF00) >> 8;
         low = CURRENT_LATCHES.IR & 0xFF;
         IMM = CURRENT_LATCHES.IR & 0xF;
  if ( GetLSHF1(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
           {  
              if ( GetADDR2MUX(CURRENT_LATCHES.MICROINSTRUCTION) == 0)
                 {
                    ADDER1 = 0;
                  }
              else if (GetADDR2MUX(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
                 {
                    if ((CURRENT_LATCHES.IR & 0x20) == 0)
                      {
                       ADDER1 = (sext6(CURRENT_LATCHES.IR & 0x3F)) << 1;
                      }
                    if ((CURRENT_LATCHES.IR & 0x20) !=0)
                      {
                       ADDER1 = (sext6(CURRENT_LATCHES.IR & 0x3F) | 0xFFFF0000) << 1;
                       }
                  }
              else if ( GetADDR2MUX(CURRENT_LATCHES.MICROINSTRUCTION) == 2)
                 {
                    if ((CURRENT_LATCHES.IR & 0x100) == 0)
                      {
                        ADDER1 = (sext9(CURRENT_LATCHES.IR & 0x1FF)) << 1; //printf("%x\n",CURRENT_LATCHES.IR);
                      }
                    if ((CURRENT_LATCHES.IR & 0x100) !=0)
                      {
                       ADDER1 = (sext9(CURRENT_LATCHES.IR & 0x1FF) | 0xFFFF0000) << 1;
                       }
                 }
              else if ( GetADDR2MUX(CURRENT_LATCHES.MICROINSTRUCTION) == 3)
                 {
                    if ((CURRENT_LATCHES.IR & 0x400) == 0)
                      { 
                        ADDER1 = (sext11(CURRENT_LATCHES.IR & 0x7FF)) << 1; 
                      }
                    if ((CURRENT_LATCHES.IR & 0x400) !=0)
                      {
                       ADDER1 = (sext11(CURRENT_LATCHES.IR & 0x7FF) | 0xFFFF0000) << 1;
                       }
                 } 
            }
         if ( GetLSHF1(CURRENT_LATCHES.MICROINSTRUCTION) == 0)
           {
              if ( GetADDR2MUX(CURRENT_LATCHES.MICROINSTRUCTION) == 0)
                 {
                    ADDER1 = 0;
                  }
              else if (GetADDR2MUX(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
                 {
                    if ((CURRENT_LATCHES.IR & 0x20) == 0)
                      {
                       ADDER1 = (sext6(CURRENT_LATCHES.IR & 0x3F)) ;
                      }
                    if ((CURRENT_LATCHES.IR & 0x20) !=0)
                      {
                       ADDER1 = (sext6(CURRENT_LATCHES.IR & 0x3F) | 0xFFFF0000) ;
                       }
                  }
              else if ( GetADDR2MUX(CURRENT_LATCHES.MICROINSTRUCTION) == 2)
                 {
                    if ((CURRENT_LATCHES.IR & 0x100) == 0)
                      {
                        ADDER1 = (sext9(CURRENT_LATCHES.IR & 0x1FF)) ; //printf("%x\n",CURRENT_LATCHES.IR);
                      }
                    if ((CURRENT_LATCHES.IR & 0x100) !=0)
                      {
                       ADDER1 = (sext9(CURRENT_LATCHES.IR & 0x1FF) | 0xFFFF0000) ;
                       }
                 }
              else if ( GetADDR2MUX(CURRENT_LATCHES.MICROINSTRUCTION) == 3)
                 {
                    if ((CURRENT_LATCHES.IR & 0x400) == 0)
                      { 
                        ADDER1 = (sext11(CURRENT_LATCHES.IR & 0x7FF)) ; 
                      }
                    if ((CURRENT_LATCHES.IR & 0x400) !=0)
                      {
                       ADDER1 = (sext11(CURRENT_LATCHES.IR & 0x7FF) | 0xFFFF0000) ;
                       }
                 } 
            }
         if ( GetADDR1MUX(CURRENT_LATCHES.MICROINSTRUCTION) == 0)
            {
               ADDER2 = CURRENT_LATCHES.PC;
            }
         if ( GetADDR1MUX(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
            {
               if (GetSR1MUX(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
                  {
                    ADDER2 = CURRENT_LATCHES.REGS [getsr1(high,low)];
                  }
               else if (GetSR1MUX(CURRENT_LATCHES.MICROINSTRUCTION) == 0)
                  {
                    ADDER2 = CURRENT_LATCHES.REGS [getdr(high)];
                  }
             }         
  if (GetMARMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 0)
     {
          MARMUX2 = ((CURRENT_LATCHES.IR & 0xFF) << 1) & 0xFFFF;
     }
  else if (GetMARMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
     {
         
         MARMUX2 = (ADDER1 + ADDER2) & 0xFFFF; 
     }
   
     
          if ( GetSR1MUX(CURRENT_LATCHES.MICROINSTRUCTION) == 0)
             {
               ADDER3 = CURRENT_LATCHES.REGS [getdr(high)]; 
             }
          if ( GetSR1MUX(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
             {
               ADDER3 = CURRENT_LATCHES.REGS [getsr1(high,low)];//printf("%x\n",ADDER3);
             }
          if ( GetSR1MUX(CURRENT_LATCHES.MICROINSTRUCTION) == 2)
             {
               ADDER3 = CURRENT_LATCHES.REGS [6];//printf("%x\n",ADDER3);
             }
          if ((CURRENT_LATCHES.IR & 0x20) == 0)
             {
               ADDER4 = CURRENT_LATCHES.REGS [CURRENT_LATCHES.IR & 0x7];
             }
          if ((CURRENT_LATCHES.IR & 0x20) != 0)
             {
               ADDER4 = sext5(CURRENT_LATCHES.IR & 0x1F); //printf("%x\n",ADDER4);printf("hello");
             }
          if (GetALUK(CURRENT_LATCHES.MICROINSTRUCTION) == 0)
             { 
               ALU2 = (ADDER3 + ADDER4) & 0xFFFF;//printf("%x\n",ADDER3);
             }
          if (GetALUK(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
             { 
               ALU2 = (ADDER3 & ADDER4) & 0xFFFF;
             }
          if (GetALUK(CURRENT_LATCHES.MICROINSTRUCTION) == 2)
             { 
               ALU2 = (ADDER3 ^ ADDER4) & 0xFFFF;
             }
          if (GetALUK(CURRENT_LATCHES.MICROINSTRUCTION) == 3)
             { 
               ALU2 = ADDER3;
             }
          if ((CURRENT_LATCHES.IR & 0x10) == 0)
             {
                SHF2 = (ADDER3 << IMM) & 0xFFFF;
              }
          else if ((CURRENT_LATCHES.IR & 0x20) == 0)
             {
                SHF2 = ((ADDER3 & 0xFFFF) >> IMM) & 0xFFFF;
             }
          else 
             {
                if ((ADDER3 & 0x8000) == 0)
                {
                   SHF2 = ((ADDER3 & 0xFFFF) >> IMM) & 0xFFFF;
                 }
                else 
                   SHF2 = ((ADDER3 | 0xFFFF0000) >> IMM) & 0xFFFF;
             }
          if (GetDATA_SIZE(CURRENT_LATCHES.MICROINSTRUCTION) == 0)
             {    
                  if ((CURRENT_LATCHES.MAR & 0x1) == 1)
                     {
                       MDR2 = (sext8((CURRENT_LATCHES.MDR & 0xFF00) >> 8)) & 0xFFFF;
                     }
                  if ((CURRENT_LATCHES.MAR & 0x1) == 0)
                     {
                       MDR2 = (sext8(CURRENT_LATCHES.MDR & 0xFF)) & 0xFFFF;
                     }
             }
           if (GetDATA_SIZE(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
              {
                   MDR2 = (CURRENT_LATCHES.MDR) & 0xFFFF;
              } 
          PC2 = CURRENT_LATCHES.PC;
                        
          if (GetR_W(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
              {   
                   if (GetDATA_SIZE(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
                      { 
                         MEMORY[CURRENT_LATCHES.MAR>>1][0] = (CURRENT_LATCHES.MDR & 0xFF) & 0xFF;
                         MEMORY[CURRENT_LATCHES.MAR>>1][1] = ((CURRENT_LATCHES.MDR & 0xFF00)>>8) & 0xFF;
                      }
                   if (GetDATA_SIZE(CURRENT_LATCHES.MICROINSTRUCTION) == 0)
                      {   
                         if ((CURRENT_LATCHES.MAR & 0x1) == 0)
                         {//printf("%d\n",(CURRENT_LATCHES.REGS[getsr1(high,low)]+sext6(low&0x3F))<<1);
                         MEMORY[CURRENT_LATCHES.MAR>>1][0] = (CURRENT_LATCHES.MDR & 0xFF) & 0xFF;}
                         if ((CURRENT_LATCHES.MAR & 0x1) == 1)
                         {
                            MEMORY[CURRENT_LATCHES.MAR>>1][1] = (CURRENT_LATCHES.MDR & 0xFF) & 0xFF;
                         }
                      }
               }
         if (GetSPMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 0)
              {
                  SP2 =  CURRENT_LATCHES.REGS[6]+2;
              }
         if (GetSPMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
              {
                  SP2 =  CURRENT_LATCHES.REGS[6]-2;
              }
         if (GetSPMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 2)
              {
                  SP2 =  CURRENT_LATCHES.USP;
              }
         if (GetSPMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 3)
              {
                  SP2 =  CURRENT_LATCHES.SSP;
              } 
         if (GetLD_VECT(CURRENT_LATCHES.MICROINSTRUCTION) ==1)
              {
                 if (GetVECTMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 0)
                    {
                       NEXT_LATCHES.VECT = (CURRENT_LATCHES.EXCV<<1) | 0x0200;
                    }
                 if (GetVECTMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
                    {
                       NEXT_LATCHES.VECT = (CURRENT_LATCHES.INTV<<1) | 0x0200;
                    }
              }
		 if (GetLD_PFN(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
		 {
			 pte2 = CURRENT_LATCHES.PTE & PTE_PFN_MASK;
		 }
		 else 
		 {
			 pte2 = CURRENT_LATCHES.PTE;
         }
         
		 if (GetLD_VPN(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
		 {
			 va2 = ((CURRENT_LATCHES.VA & 0xFE00) << 1) >> 9;
		 }
		 else 
		 {
			 va2 = CURRENT_LATCHES.VA & PAGE_OFFSET_MASK;
		 }
		 
		 




			 
}


void drive_bus() {

  /* 
   * Datapath routine for driving the bus from one of the 5 possible 
   * tristate drivers. 
   */       
   if (GetGATE_MDR(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
     {
       BUS = MDR2;
      }
   else if (GetGATE_PC(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
     {
       BUS = PC2;
      }
   else if (GetGATE_MARMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
     {
       BUS = MARMUX2;
      }
   else if (GetGATE_ALU(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
     {
       BUS = ALU2;
      }
   else if (GetGATE_SHF(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
     {
       BUS = SHF2;
      }
   else if (GetGate_SP(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
     {
       BUS = SP2;
     }
   else if (GetGate_PSR(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
     {  
       BUS = CURRENT_LATCHES.PSR;
     }
   else if (GetGate_VECT(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
     {  
       BUS = CURRENT_LATCHES.VECT;
     }
   else if (GetGatePC1(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
     {  
       BUS = CURRENT_LATCHES.PC -2;
     }
   else if (GetGatePTE(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
   {
	   if (GetGateVA(CURRENT_LATCHES.MICROINSTRUCTION)==1)
	   {
	      BUS = pte2 | va2;
	   }
	   else 
	   {
		   BUS = pte2;
	   }
   }
   else if (GetGatePTBR(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
   {
	   BUS = (CURRENT_LATCHES.PTBR & 0xFF00) + va2;
   }
   else if (GetGateMAR(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
   {
	   BUS = CURRENT_LATCHES.MAR;
   }
   else if (GetGatePSRSAVED(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
   {
	   BUS = CURRENT_LATCHES.PSRSAVED;
   }
   else BUS = 0;
}


void latch_datapath_values() {

  /* 
   * Datapath routine for computing all functions that need to latch
   * values in the data path at the end of this cycle.  Some values
   * require sourcing the bus; therefore, this routine has to come 
   * after drive_bus.
   */       
int mux;
//if (GetGATE_MDR (CURRENT_LATCHES.MICROINSTRUCTION) == 1)
   //{
      if (GetMIO_EN(CURRENT_LATCHES.MICROINSTRUCTION) == 0)
         {
            if (GetDATA_SIZE(CURRENT_LATCHES.MICROINSTRUCTION) == 0)
             {
                  if ((CURRENT_LATCHES.MAR & 0x1) == 1)
                     {
                       mux = (sext8(BUS & 0xFF) ) & 0xFFFF;
                     }
                  if ((CURRENT_LATCHES.MAR & 0x1) == 0)
                     {
                       mux = (sext8(BUS & 0xFF)) & 0xFFFF;
                     }
             }
           if (GetDATA_SIZE(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
              {
                   mux = BUS & 0xFFFF;
              } 
          }        
       if (GetMIO_EN(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
         {
             mux = temp; 
         }

       if (GetLD_MDR(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
         {
            NEXT_LATCHES.MDR = mux;//printf("%x\n",mux);
         }
       if (GetLD_MAR(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
         {
            NEXT_LATCHES.MAR = BUS;
         }
       if (GetLD_PC(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
         {
           if ( GetPCMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 0 )
              {
                NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2;
              }
           if ( GetPCMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 1 )
              {
                NEXT_LATCHES.PC = BUS;
              }
           if ( (GetPCMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 2) )
              {
                NEXT_LATCHES.PC = ADDER1 + ADDER2;
              }
         }
       if (GetLD_IR(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
         {
            NEXT_LATCHES.IR = BUS; //printf("hello");
         }
       if (GetLD_BEN(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
         {
            NEXT_LATCHES.BEN = (CURRENT_LATCHES.N & ((CURRENT_LATCHES.IR & 0x800)>>11)) | (CURRENT_LATCHES.Z & ((CURRENT_LATCHES.IR & 0x400)>>10)) | (CURRENT_LATCHES.P & ((CURRENT_LATCHES.IR & 0x200)>>9));//printf("%d\n", NEXT_LATCHES.BEN);
         }
if (GetLD_PSR15(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
         {
              NEXT_LATCHES.PSR = ((CURRENT_LATCHES.PSR<<1) & 0xFFFF)>>1;
         }


       if (GetLD_CC(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
         {  
	        if(GetLD_PSRNZP(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
			{
			  NEXT_LATCHES.N = (BUS & 0x4)>>2;
              NEXT_LATCHES.Z = (BUS & 0x2)>>1;
              NEXT_LATCHES.P = BUS & 0x1;
			}
			
           else if ( (BUS & 0x8000) == 0x8000)
             {
              NEXT_LATCHES.N = 1;
              NEXT_LATCHES.Z = 0;
              NEXT_LATCHES.P = 0;
              NEXT_LATCHES.PSR = ((CURRENT_LATCHES.PSR>>3)<<3) | 0x4;
             }
            else if ( ((BUS & 0x8000) == 0) && (BUS != 0))
             {
              NEXT_LATCHES.N = 0;
              NEXT_LATCHES.Z = 0;
              NEXT_LATCHES.P = 1;
              NEXT_LATCHES.PSR = ((CURRENT_LATCHES.PSR>>3)<<3) | 0x1;
             }
            else 
             {
              NEXT_LATCHES.N = 0;
              NEXT_LATCHES.Z = 1;
              NEXT_LATCHES.P = 0;
              NEXT_LATCHES.PSR = ((CURRENT_LATCHES.PSR>>3)<<3) | 0x2;
             }

          }
        if (GetLD_REG(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
          {
             if (GetDRMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 0)
              NEXT_LATCHES.REGS [getdr(high)] = BUS;
             if (GetDRMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
              NEXT_LATCHES.REGS [7] = BUS;
             if (GetDRMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 2)
              NEXT_LATCHES.REGS [6] = BUS;
          }
      if (GetLD_INT(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
         {
              NEXT_LATCHES.INT = 0;
         }
     if (GetLD_USP(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
         {
              NEXT_LATCHES.USP = CURRENT_LATCHES.REGS[6];
         }
     if (GetLD_SSP(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
         {
              NEXT_LATCHES.SSP = CURRENT_LATCHES.REGS[6];
         }
     
     if (GetLD_PSR(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
         {
              NEXT_LATCHES.PSR = BUS;
         }
		
	 if ((GetLD_EXP(CURRENT_LATCHES.MICROINSTRUCTION) == 1) && (CURRENT_LATCHES.MDR&0x0001 == 1) && (GetWORDALIGN(CURRENT_LATCHES.MICROINSTRUCTION) == 1))//unaligned
         {
             
              
              NEXT_LATCHES.EXP = 1;
              NEXT_LATCHES.EXCV = 0x3;
              
         } 
     else if (((CURRENT_LATCHES.PSR & 0x8000) !=0)&&  ((mux & 0x8) == 0) && (GetPROTECTION(CURRENT_LATCHES.MICROINSTRUCTION)==1) && (CURRENT_LATCHES.LDST != 28) && (CURRENT_LATCHES.READY == 1) )//protection
         {   
   
              NEXT_LATCHES.EXP = 1;
              NEXT_LATCHES.EXCV = 0x4;
         }
	 else if ((GetEXPMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 1) && ((mux & 0x4) == 0) && (GetPROTECTION(CURRENT_LATCHES.MICROINSTRUCTION) == 1) && (CURRENT_LATCHES.READY == 1))//invalid
	    {
              NEXT_LATCHES.EXP = 1;
              NEXT_LATCHES.EXCV = 0x2;		
		}			  
     else if (((CURRENT_LATCHES.IR>>12) == 10) || ((CURRENT_LATCHES.IR>>12) == 11))//unknown
        {
           
           NEXT_LATCHES.EXCV = 0x5;
              
         } 
      if ((GetLD_EXP(CURRENT_LATCHES.MICROINSTRUCTION) == 1) && (GetEXPMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 0))
        {
          NEXT_LATCHES.EXP = 0;
         }
      if(GetLD_INT(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
        {
          NEXT_LATCHES.INT = 0;
         }
	  if(GetLD_VA(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
	  {
		  NEXT_LATCHES.VA = BUS;
	  }
	  if(GetLD_PTE(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
	  {
		  NEXT_LATCHES.PTE = BUS;
	  }
	  if((GetLD_MD(CURRENT_LATCHES.MICROINSTRUCTION) == 1) && (GetMDMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 1))
	  {
		  NEXT_LATCHES.MD = 1;
	  }
	  if((GetLD_MD(CURRENT_LATCHES.MICROINSTRUCTION) == 1) && (GetMDMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 0))
	  {
		  NEXT_LATCHES.MD = 0;
	  }
	  
	  if((GetLD_PTER(CURRENT_LATCHES.MICROINSTRUCTION) == 1))
	  {
		  if((CURRENT_LATCHES.MD == 1) && (GetLD_PTEM(CURRENT_LATCHES.MICROINSTRUCTION) == 1) )
	    {
		  NEXT_LATCHES.PTE = CURRENT_LATCHES.PTE | 0x3; 
	    }
		else NEXT_LATCHES.PTE = CURRENT_LATCHES.PTE | 0x1;
	  }
	  if(GetLD_LDST(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
	  {
		  switch(GetLDSTMUX(CURRENT_LATCHES.MICROINSTRUCTION))
		  {
            case 1:
                NEXT_LATCHES.LDST = 29;
				break;
            case 2:
                NEXT_LATCHES.LDST = 25;	
				break;
            case 3:
                NEXT_LATCHES.LDST = 23;
				break;
            case 4:
                NEXT_LATCHES.LDST = 24;		
				break;
            case 5:
                NEXT_LATCHES.LDST = 33;
				break;
            case 6:
                NEXT_LATCHES.LDST = 46;
				break;
            case 7:
                NEXT_LATCHES.LDST = 49;
				break;
            case 8:
                NEXT_LATCHES.LDST = 38;
				break;
            case 9:
                NEXT_LATCHES.LDST = 40;
				break;
            case 10:
                NEXT_LATCHES.LDST = 43;	
                break;				
            case 11:
                NEXT_LATCHES.LDST = 28;	
                break;
            default: 
                break;
		  }
	  }
     if (GetPSRSAVED(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
	 {
		 NEXT_LATCHES.PSRSAVED = CURRENT_LATCHES.PSR;
	 }
	 
        		 

}
