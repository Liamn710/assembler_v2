#ifndef FIRST_PASS
#define FIRST_PASS

#define MAX_LABELS 100 
#define MAX_LABEL_LEN 50 
#define START_ADDRESS 100
#define MAX_LINE_LENGTH 100
#define INITIAL_LINES 100

struct opcode {
    char *name;         /* Opcode name */
    int num_operands;  /* Number of operands required by the opcode */
};
extern struct opcode opcode_table[16];  /* Table of opcodes */
extern char *instructions_table[];      /* List of assembler directives */
extern char *registers[];               /* List of register names */

typedef struct {
    char name[MAX_LABEL_LEN]; /* todo - maybe explicit size? */
    int address;
} label_entry;

/* Function prototypes for user's existing functions */
int is_register(const char* operand);
int is_opcode(const char* opcode);

#endif