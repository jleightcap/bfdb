#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "interpreter.h"

// Read .bf file, populating array of instructions.
void instr_init(char* path, struct instructions *parse) {
    FILE* fd = fopen(path, "r");
    if (fd == NULL) {
        fprintf(stderr, "Cannot open %s.\n", path);
        exit(1);
    }

    // first pass of file: count number of valid instructions
    int instr_count = 0;
    char c = getc(fd);
    while(c != EOF) {
        if (c == '+' || c == '-' || c == '<' || c == '>' ||
            c == ',' || c == '.' || c == '[' || c == ']')
            instr_count++;
        c = getc(fd);
    }
    // initialize instruction array size to number of valid instructions
    char* instr = (char*)malloc(instr_count * sizeof(char));

    // second pass of file: populate instruction array
    instr_count = 0;
    rewind(fd);
    c = getc(fd);
    while(c != EOF) {
        if (c == '+' || c == '-' || c == '<' || c == '>' ||
            c == ',' || c == '.' || c == '[' || c == ']') {
            instr[instr_count] = c;
            instr_count++;
        }
        c = getc(fd);
    }

    parse->fd = fd;
    parse->instr_count = instr_count;
    parse->instr = instr;
}

// free memory and close file
void instr_exit(struct instructions* parse) {
    fclose(parse->fd);
    free(parse->instr);
}

// clear/initialize execution state
// no paired free, no dynamic memory allocated or file
void exec_init(struct exec_state* es) {
    memset(es->tape, 0, TAPE_LENGTH * sizeof(byte));
    es->inst_ptr    = 0;
    es->data_ptr    = 0;
    es->cycle       = 0;
    es->data_extent = 0;
    es->bp_ptr      = -1;
    es->ee          = INIT;
}

enum exec_end exec(struct instructions* instr, struct exec_state* es) {
    while(es->cycle < MAX_CYCLES) {
        // instruction pointer at breakpoint
        if (es->inst_ptr == es->bp_ptr) {
            return BREAK;
        }

        // temporary byte to hold user input on ,
        byte c;
        // temporary initial jump location stored to maintain steps
        int jump_from; 
        // in place of stack, maintain 'balance' of braces:
        // a pair matches if brackets are equal.
        int balance;
                       

        switch(instr->instr[es->inst_ptr]) {
            case '+':
                es->tape[es->data_ptr]++;
                es->inst_ptr++;
                break;

            case '-':
                es->tape[es->data_ptr]--;
                es->inst_ptr++;
                break;

            case '<':
                es->data_ptr--;
                es->inst_ptr++;
                break;

            case '>':
                es->data_ptr++;
                es->inst_ptr++;
                break;

            case '.':
                printf("%c\n", es->tape[es->data_ptr]);
                es->inst_ptr++;
                break;

            case ',':
                scanf(" %c", &c);
                es->tape[es->data_ptr] = c;
                es->inst_ptr++;
                break;

            case '[':
                jump_from = es->inst_ptr;
                if (es->tape[es->data_ptr] == 0) {
                    balance = 1;
                    while (balance != 0) {
                        es->inst_ptr++;
                        if (es->inst_ptr > instr->instr_count) {
                            printf("Instuction overflow at instruction %d\n",
                                    es->inst_ptr - 1);
                            return OVERFLOW;
                        }

                        if (instr->instr[es->inst_ptr] == '[')
                            balance++;
                        if (instr->instr[es->inst_ptr] == ']')
                            balance--;
                    }
                    // if breakpoint was at next instuction,
                    // increase to instruction after jump.
                    // Used for stepping through loops.
                    if (jump_from == es->bp_ptr + 1)
                        es->bp_ptr = es->inst_ptr + 1;
                }
                else es->inst_ptr++;
                break;

            case ']':
                jump_from = es->inst_ptr;
                if (es->tape[es->data_ptr] != 0) {
                    balance = -1;
                    while (balance != 0) {
                        es->inst_ptr--;
                        if (es->inst_ptr < 0) {
                            printf("Instruction underflow at instruction %d\n",
                                    es->inst_ptr + 1);
                            return UNDERFLOW;
                        }

                        if (instr->instr[es->inst_ptr] == '[')
                            balance++;
                        if (instr->instr[es->inst_ptr] == ']')
                            balance--;
                    }
                    // if breakpoint was at next instuction,
                    // increase to instruction after jump.
                    // Used for stepping through loops.
                    if (jump_from == es->bp_ptr + 1)
                        es->bp_ptr = es->inst_ptr + 1;
                }
                else es->inst_ptr++;
                break;

        }

        // successfull termination
        if (instr->instr_count == es->inst_ptr) {
            printf("Program terminated successfully.\n");
            return VALID;
        }

        // data conditions
        if (es->data_ptr < 0) {
            printf("Data underflow at instruction %d.\n", es->inst_ptr);
            return OVERFLOW;
        }
        if (es->data_ptr > TAPE_LENGTH) {
            printf("Data overflow at instruction %d.\n", es->inst_ptr);
            return UNDERFLOW;
        }
        if (es->data_ptr > es->data_extent)
            es->data_extent = es->data_ptr;

        es->cycle++;
    }

    printf("Cycle limit reached.\n");
    return CYCLE_LIMIT;
}
