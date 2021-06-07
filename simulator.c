/* LC-2K Instruction-level simulator */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define NUMMEMORY 65536 /* maximum number of words in memory */
#define NUMREGS 8 /* number of machine registers */
#define MAXLINELENGTH 1000
#define OPCODE_MASK 0x0E000000
#define REG_MASK 0x00000007
#define OFF_MASK 0x0000FFFF

typedef struct stateStruct {
	int pc;
	int mem[NUMMEMORY];
	int reg[NUMREGS];
	int numMemory;
} stateType;

void printState(stateType *);
int convertNum(int num);

int main(int argc, char *argv[]){ 
	char line[MAXLINELENGTH];
	stateType state;	
	FILE *filePtr;
	
	if (argc != 2) {
		printf("error: usage: %s <machine-code file>\n", argv[0]);
		exit(1);
	}
	
	filePtr = fopen(argv[1], "r");
	if (filePtr == NULL) {
		printf("error: can't open file %s", argv[1]);
		perror("fopen");
		exit(1);
	}
	
	/* read in the entire machine-code file into memory */
	for (state.numMemory = 0; fgets(line, MAXLINELENGTH, filePtr) != NULL; state.numMemory++) {
		if (sscanf(line, "%d", state.mem+state.numMemory) != 1) {
			printf("error in reading address %d\n", state.numMemory);
			exit(1);
		}
		printf("memory[%d]=%d\n", state.numMemory, state.mem[state.numMemory]);
	}
	
	if (!flag) { // error files in assembly
		printf("error in reading address %d\n", state.numMemory);
    	exit(1);
	}
	
	//start with pc = 0
	state.pc = 0;
	//allocate registers to 0
	memset(state.reg, 0, (sizeof(int) * NUMREGS));
	
	int halted = 0;
	int inst_count = 0;
	int regA, regB, destReg, offset, addr;		
	
	while (state.pc > MAXLINELENGTH || *(state.mem) > NUMMEMORY){
		regA = regB = destReg = offset = addr = 0; //clear variables
		int inst = state.mem[state.pc]; //fetch decimal inst from memory
		int opcode = inst & OPCODE_MASK;
		
		printState(&state); 
		
		//parse the instruction for registers and offset
		regA = (inst >> 19) & REG_MASK;
		regB = (inst >> 16) & REG_MASK;
		destReg = inst & REG_MASK;
		offset = inst & OFF_MASK;
		
		//execute opcode 
		switch(opcode){
			case 0: 
				//Add contents of regA with contents of regB, store results in destReg.
				state.reg[destReg] = state.reg[regA] + state.reg[regB];
				break;
				
			case 1:
				//Nor contents of regA with contents of regB, store results in destReg.
				state.reg[destReg] = ~(state.reg[regA] | state.reg[regB]);
				break;
			
			case 2:
				//Load regB into memory. Memory address is formed by adding offsetField with the contents of regA.
				addr = convertNum(offset + state.reg[regA]); 
				state.reg[regB] = state.mem[addr];
				break;
				
			case 3:
				//Store regB into memory. Memory address is formed by adding offsetField with the contents of regA.
				addr = convertNum(offset + state.reg[regA]); 
				state.mem[addr] = state.reg[regB]; 
				break;
				
			case 4:
				//If the contents of regA and regB are the same, then branch to the address PC+1+offsetField.
				if(state.reg[regA] == state.reg[regB])
					state.pc = state.pc+convertNum(offset);
				break;
			
			case 5:
				//First store PC+1 into regB, where PC is the address of this jalr instruction.
				state.reg[regB] = state.pc + 1;
				//Then branch to the address contained in regA.
				state.pc = state.reg[regA] -1;
				break;
				
			case 6:
				//Increment the PC (as with all instructions), then halt the machine.
				state.pc++;
				halted = 1;
				break;
				
			case 7:
				break;
				
			default:
				printf("unknown opcode\n");
				break;
			}
			
			state.pc++;
			inst_count++;
		}
	printf("\nmachine halted\n");
	printf("total of %d instructions executed\n", inst_count);
	printf("final state of machine:\n");
		
	printState(&state);
	
	fclose(filePtr);
	exit(0);
}

void printState(stateType *statePtr){
	int i;
	
	printf("\n@@@\nstate:\n");
	printf("\tpc %d\n", statePtr->pc);
	printf("\tmemory:\n");
	
	for (i=0; i<statePtr->numMemory; i++) {
		printf("\t\tmem[ %d ] %d\n", i, statePtr->mem[i]);
	}
	
	printf("\tregisters:\n");
	
	for (i=0; i<NUMREGS; i++) {
		printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
	}
	
	printf("end state\n");
}

int convertNum(int num)
{
	/* convert a 16-bit number into a 32-bit Linux integer */
	if (num & (1<<15) ) {
		num -= (1<<16);
	}
	return(num);
}
