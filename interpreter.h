#ifndef _INTERPRETER
#define _INTERPRETER

#include <stdio.h>

#define TAPE_LENGTH 30000     // number of byte cells
#define MAX_CYCLES  100000000 // maximum execution cycles

struct instructions {
    FILE* fd; // stored to later free
    int instr_count;
    char* instr;
} instrucitons;

// instruction initialization
void instr_init(char* path, struct instructions *parse);
void instr_exit();



typedef unsigned char byte;

// return states of bf program
enum exec_end { INIT, VALID, BREAK, UNDERFLOW, OVERFLOW, CYCLE_LIMIT };

struct exec_state {
    byte tape[TAPE_LENGTH];      // state of the tape

    int  inst_ptr;    // state of the instruction pointer
    int  data_ptr;    // state of the data pointer
    int  bp_ptr;      // pointer to instruction breakpoint
    int  cycle;       // current cycle
    int  data_extent; // highest data cell used
    enum exec_end ee;
} exec_state;


void exec_init(struct exec_state* es);
enum exec_end exec(struct instructions* parse, struct exec_state* es);


#endif
