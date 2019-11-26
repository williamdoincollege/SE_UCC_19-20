#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define STACK_SIZE 256
static int stack[STACK_SIZE];

/** Instructions */
enum instructions{
    HLT, // 0        :: halts program
    PSH, // 1        :: pushes <val> to stack
    POP, // 2        :: pops value from stack
    ADD, // 3        :: adds top two vals on stack
    MUL, // 4        :: multiplies top two vals on stack
    DIV, // 5        :: divides top two vals on stack
    SUB, // 6        :: subtracts top two vals on stack
};

/** Registers */
enum registers{
    A, B, C, D, E, F, I, J, // general purpose
    IP,                     // instruction pointer
    SP,                     // stack pointer
    REGISTER_SIZE
};

static int registers[REGISTER_SIZE];

// instructions array
int *instructions;

// how many instructions to do
int instruction_count = 0;

// space allocated for the instructions(default = 4)
int instruction_space = 4;

static bool running = true;    // if the program is running

/** quick ways to get SP and IP */
#define SP (registers[SP])
#define IP (registers[IP])

/** fetch current instruction set */
#define FETCH (instructions[IP])

/** prints the stack from A to B */
void print_stack() {
    for (int i = 0; i < SP; i++) {
        printf("0x%04d ", stack[i]);
        if ((i + 1) % 4 == 0) { printf("\n"); }
    }
    if (SP != 0) { printf("\n"); }
}

void print_registers() {
    printf("Register Dump:\n");
    for (int i = 0; i < REGISTER_SIZE; i++) {
        printf("%04d ", registers[i]);
        if ((i + 1) % 4 == 0) { printf("\n"); }
    }
}

void eval(int instr) {
    switch (instr) {
        case HLT: {
            running = false;
            printf("Finished Execution\n");
            break;
        }
        case PSH: {
            SP = SP + 1;
            IP = IP + 1;
            stack[SP] = instructions[IP];
            break;
        }
        case POP: {
            SP = SP - 1;
            break;
        }
        case ADD: {
            registers[A] = stack[SP];
            SP = SP - 1;

            registers[B] = stack[SP];    // SP = SP - 1
            registers[C] = registers[B] + registers[A];

            
            stack[SP] = registers[C];    // SP = SP + 1 
            printf("%d + %d = %d\n", registers[B], registers[A], registers[C]);
            break;
        }
        case MUL: {
            registers[A] = stack[SP];
            SP = SP - 1;

            registers[B] = stack[SP];    //SP = SP - 1
            registers[C] = registers[B] * registers[A];
          
            stack[SP] = registers[C];    //SP = SP + 1
            printf("%d * %d = %d\n", registers[B], registers[A], registers[C]);
            break;
        }
        case DIV: {
            registers[A] = stack[SP];
            SP = SP - 1;

            registers[B] = stack[SP];    // SP = SP - 1
            registers[C] = registers[B] / registers[A];

            stack[SP] = registers[C];    // SP = SP + 1
            printf("%d / %d = %d\n", registers[B], registers[A], registers[C]);
            break;
        }
        case SUB: {
            registers[A] = stack[SP];
            SP = SP - 1;

            registers[B] = stack[SP];    // SP = SP - 1
            
            registers[C] = registers[B] - registers[A];

            stack[SP] = registers[C];    //* SP = SP + 1 
            printf("%d - %d = %d\n", registers[B], registers[A], registers[C]);
            break;
        }

        default: {
            printf("Unknown Instruction %d\n", instr);
            break;
        } 
    }
}

int main(int argc, char** argv) {
    if (argc <= 1) {
        printf("error: no input files\n");
        return -1;
    }

    char *filename = argv[1];       // filename  

    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("error: could not read file `%s`\n", filename);
        return -1;
    }
    
    // allocate space for instructions
    instructions = malloc(sizeof(*instructions) * instruction_space);

    // read the "binary" file
    int num;
    int i = 0;
    while (fscanf(file, "%d", &num) > 0) {
        instructions[i] = num;
        printf("%d\n", instructions[i]);
        i++;
        if (i >= instruction_space) {
            instruction_space *= 2;
            instructions = realloc(instructions, sizeof(*instructions) * instruction_space); // double size
        }
    }
    
    // set 'instruction_count' to number of instructions read
    instruction_count = i;

    // close the file
    fclose(file);

    // initialize stack pointer
    SP = -1;

    // loop through program, witout going out of the programs bounds
    // 
    while (running && IP < instruction_count) {
        eval(FETCH);{
            IP = IP + 1;
        }
    }

    // clean up instructions
    free(instructions);

    return 0;
}
