/* Assembler code fragment for LC-2K */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAXLINELENGTH 1000
#define MAXNUMMEMORY 65536

//opcode in binary (specified in project documentation)
#define OP_ADD  0x00000000
#define OP_NOR  0x00400000
#define OP_LW   0x00800000
#define OP_SW   0x00C00000
#define OP_BEQ  0x01000000
#define OP_JALR 0x01400000
#define OP_HALT 0x01800000
#define OP_NOOP 0x01C00000

int readAndParse(FILE *, char *, char *, char *, char *, char *);
int isNumber(char *);

//table of labels with each not exceeding 6 char (including null char)
char label_list[MAXNUMMEMORY][7];

int findLabel(char *label, int cnt_line) {
	for (int i = 0; i < cnt_line; i++) {
		if (!strcmp(label_list[i], label)) {
			return i;
		}
	}
	return -1;
}

int main(int argc, char *argv[]){ 
	char *inFileString, *outFileString;
	FILE *inFilePtr, *outFilePtr;
	char label[MAXLINELENGTH], opcode[MAXLINELENGTH], arg0[MAXLINELENGTH],
				arg1[MAXLINELENGTH], arg2[MAXLINELENGTH];

	if (argc != 3) {
		printf("error: usage: %s <assembly-code-file> <machine-code-file>\n", argv[0]);
		exit(1);
	}
	
	inFileString = argv[1];
	outFileString = argv[2];
	
	inFilePtr = fopen(inFileString, "r");	
	if (inFilePtr == NULL) {
		printf("error in opening %s\n", inFileString);
		exit(1);
	}
	
	outFilePtr = fopen(outFileString, "w");
	if (outFilePtr == NULL) {
		printf("error in opening %s\n", outFileString);
		exit(1);
	}
	
	/*start by searching all labels in assembly file*/
	int line_cnt = 0; //number of lines in assembly file
	while(1){
		if (! readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2) ) 
			break;
			
		//ignore blank label
		if (!strcmp(label, "")) {
			line_cnt++;
			continue;
		}
		
		//max label char is 6 and must start with letter
		if (strlen(label) > 6 || !isalpha(label[0]) ) {
			printf("error: invalid label name: %s\n", label);
			exit(1);
		}
		
		//duplicate lables
		if(findLabel(label, line_cnt) != -1) {
			printf("error: duplicate label: %s\n", label);
			exit(1);
		}
		
		//insert label into lable list
		strcpy(label_list[line_cnt++], label);
	}

	/* this is how to rewind the file ptr so that you start reading from the
	beginning of the file */
	rewind(inFilePtr);
	
	/* after doing a readAndParse, you may want to do the following to test the
	opcode */
	
	int memory[MAXNUMMEMORY];
	int regA,regB,offset,op_code,mach_code; //mach_code = machine code without opcode (bit 24-22)
	
	int idx = 0;
	while (1) {
		if (! readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2) ) 
			break;
			
		regA = regB = offset = op_code = mach_code =0;
		
		//R-Type
		if (!strcmp(opcode, "add") || !strcmp(opcode, "nor")) {
			if (!(isNumber(arg0) && isNumber(arg1) && isNumber(arg2))) {
				printf("error: invalid argument of R-Type: line %d.\n",idx);	
				exit(1);
			}
				
			if (!strcmp(opcode, "add")) 
				op_code = OP_ADD;
			else if (!strcmp(opcode, "nor")) 
				op_code = OP_NOR;

			regA = atoi(arg0) << 19; //push arg0 to bit 21-19 (regA)
			regB = atoi(arg1) << 16; //push arg0 to bit 18-16 (regB)
			offset = atoi(arg2);     //dest reg
				
			mach_code = regA | regB | offset;
		}
				
		//I-Type
		else if (!strcmp(opcode, "lw") || !strcmp(opcode, "sw") || !strcmp(opcode, "beq")) {
			if (!(isNumber(arg0) && isNumber(arg1)) || !strcmp(arg2,"")) {
				printf("error: invalid argument of I-Type: line %d.\n",idx);	
				exit(1);
			}
				
			if (!strcmp(opcode, "lw")) 
				op_code = OP_LW;
			else if (!strcmp(opcode, "sw")) 
				op_code = OP_SW;
			else if (!strcmp(opcode, "beq")) 
				op_code = OP_BEQ;
					
			regA = atoi(arg0) << 19; //push arg0 to bit 21-19 (regA)
			regB = atoi(arg1) << 16; //push arg0 to bit 18-16 (regB)	
				
			//numeric address for lw,sw
			if(isNumber(arg2)){
				offset = atoi(arg2);
				//check offset range
				if (offset < -32768 || offset > 32767) {
					printf("error: invalid offsetField range\n");
					exit(1);
				}
			}
			//arg2 is not number (jump label for beq)
			else{
				if ((offset = findLabel(arg2,line_cnt)) == -1){
					printf("error: label not found for beq:%s : line %d\n",arg2,idx);
					exit(1);
				}
				if (op_code == OP_BEQ)
					offset -= (idx+1);
				}
			
			//take last 16 bits for offset
			if (op_code == OP_BEQ)
				offset &= 0x0000FFFF;
					
			mach_code = regA | regB | offset;
		}

				
			//J-Type
		else if (!strcmp(opcode, "jalr")) {
			if (!(isNumber(arg0) && isNumber(arg1))) {
				printf("error: invalid argument of J-Type: line %d.\n",idx);	
				exit(1);
			}
					
			op_code = OP_JALR;
			regA = atoi(arg0) << 19; //push arg0 to bit 21-19 (regA)
			regB = atoi(arg1) << 16; //push arg0 to bit 18-16 (regB)	
					
			mach_code = regA | regB;
		}
			
			//O-Type
		else if (!strcmp(opcode, "halt") || !strcmp(opcode, "noop")) {
			if (!strcmp(opcode, "halt")) 
				op_code = OP_HALT;
			else if (!strcmp(opcode, "noop")) 
				op_code = OP_NOOP;
		}
			
		//.fill
		else if (!strcmp(opcode, ".fill")) {
			if(!strcmp(arg0,"")){
				printf("error: invalid argument of O-Type: line %d.\n",idx);	
				exit(1);
			}
				
			if(isNumber(arg0))
				mach_code = atoi(arg0);
					
			else{
				if ((mach_code = findLabel(arg0,line_cnt)) == -1){
					printf("error: label not found for .fill:%s\n",arg0);
					exit(1);
				}
			}
		}
			
		//unknown instruction
		else{
			printf("error: unknown instruction: %s\n",opcode);
			exit(1);
		}
		
		memory[idx++] = op_code | mach_code; //append opcode to all machine codes and save to memory
	}//while end
	
	//print all memory addresses for every line in assembly file
	//additionaly, save the result to outFile
	for(int i=0 ; i<line_cnt ; i++){
		printf("(address %d): %d (hex 0x%x)\n", i, memory[i], memory[i]);
		if(outFilePtr != NULL)
			fprintf(outFilePtr, "%d\n", memory[i]);
	}
	
	exit(0);
}

/*
* Read and parse a line of the assembly-language file. Fields are returned
* in label, opcode, arg0, arg1, arg2 (these strings must have memory already
* allocated to them).
*
* Return values:
* 0 if reached end of file
* 1 if all went well
*
* exit(1) if line is too long.
*/
int readAndParse(FILE *inFilePtr, char *label, char *opcode, char *arg0, char *arg1, char *arg2){ 
	char line[MAXLINELENGTH];
	char *ptr = line;
	/* delete prior values */
	label[0] = opcode[0] = arg0[0] = arg1[0] = arg2[0] = '\0';
	/* read the line from the assembly-language file */
	if (fgets(line, MAXLINELENGTH, inFilePtr) == NULL) {
	/* reached end of file */
		return(0);
	}
	/* check for line too long (by looking for a \n) */
	if (strchr(line, '\n') == NULL) {
	/* line too long */
		printf("error: line too long\n");
		exit(1);
	}
	/* is there a label? */
	ptr = line;
	if (sscanf(ptr, "%[^\t\n\r ]", label)) {
	/* successfully read label; advance pointer over the label */
		ptr += strlen(label);
	}
	/*
	* Parse the rest of the line. Would be nice to have real regular
	* expressions, but scanf will suffice.
	*/
	sscanf(ptr, "%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]", opcode, arg0, arg1, arg2);
	return(1);
}

int isNumber(char *string)
{ 
	/* return 1 if string is a number */
	int i;
	return( (sscanf(string, "%d", &i)) == 1);
}


