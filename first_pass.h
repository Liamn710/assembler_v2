#ifndef FIRST_PASS
#define FIRST_PASS

#define MAX_LABELS 100 
#define MAX_LABEL_LEN 50 
#define START_ADDRESS 100
#define MAX_LINE_LENGTH 100
#define INITIAL_LINES 100
#define MAX_OPCODE_LENGTH 5
#define MAX_OPERAND_LENGTH 20

struct opcode {
    char *name;         /* Opcode name */
    int num_operands;  /* Number of operands required by the opcode */
};
extern struct opcode opcode_table[16];  /* Table of opcodes */
extern char *instructions_table[];      /* List of assembler directives */
extern char *registers[];               /* List of register names */

/* Function prototypes for user's existing functions */
int is_opcode(char *str);
int is_reg(char *str);

#endif