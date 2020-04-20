#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "interpreter.h"

#define MAX_USER_INPUT 100

// helper function for number of digits of a given int
int num_digits(int nn) {
    int digits = 0;
    while (nn > 0) {
        nn /= 10;
        digits++;
    }
    return digits;
}

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: ./bfdb [FILE]\n");
        return 1;
    }

    struct instructions instr;
    struct exec_state es;
    instr_init(argv[1], &instr);
    exec_init(&es);

    char user_input[MAX_USER_INPUT];

    while(1) {
        printf("\033[1m\033[32m(bfdb)\033[m ");
        scanf("%s", user_input);

        if (strcmp(user_input, "quit") == 0)
            return 0;

        // run until breakpoint or until program returns valid
        if (strcmp(user_input, "run")  == 0) {
            // reinitalize if running after valid return
            es.ee = exec(&instr, &es);
            continue;
        }

        // reset execution state
        if (strcmp(user_input, "reset") == 0) {
            exec_init(&es);
            continue;
        }

        // break execution at breakpoint
        if (strcmp(user_input, "b") == 0) {
            int nn = num_digits(instr.instr_count);
            char bp[nn - 1];
            scanf("%s", bp);
            int bp_ptr = atoi(bp);
            if (bp_ptr < 0 || bp_ptr > instr.instr_count)
                printf("Invalid instruction number %d\n", bp_ptr);
            else
                es.bp_ptr = bp_ptr;
            continue;
        }

        // print tape index
        if (strcmp(user_input, "p") == 0) {
            int nn = num_digits(TAPE_LENGTH);
            char ii[nn - 1];
            scanf("%s", ii);
            int tape_ptr = atoi(ii);
            if (tape_ptr < 0 || tape_ptr >= TAPE_LENGTH)
                printf("Invalid tape index %d\n", tape_ptr);
            else {
                printf("tape[%d] = %x", tape_ptr, es.tape[tape_ptr]);
                // printable ASCII
                if (es.tape[tape_ptr] > 32 && es.tape[tape_ptr] < 127)
                    printf(" (%c)", es.tape[tape_ptr]);
                printf("\n");
            }
            continue;
        }

        // step single instruction (set breakpoint one instruction ahead)
        if (strcmp(user_input, "s") == 0) {
            es.bp_ptr = es.inst_ptr + 1;
            es.ee = exec(&instr, &es);
            printf("tape[%d] = %d\tinst_ptr = %d\n",
                    es.data_ptr, es.tape[es.data_ptr], es.inst_ptr);
            continue;
        }

        if (strcmp(user_input, "dump") == 0) {
            FILE* fd = fopen("dump.dat", "w");
            if (fd == NULL) {
                fprintf(stderr, "Could not create dump.dat.\n");
                return 1;
            }

            for (int ii = 0; ii < es.data_extent; ii++)
                fprintf(fd, "%x ", es.tape[ii]);

            fclose(fd);
            printf("Tape dumped to data.dat.\n");
            continue;
        }

        printf("Unrecognized command %s: see 'help'\n",
                user_input);
    }

    instr_exit(&instr);
    return 0;
}
