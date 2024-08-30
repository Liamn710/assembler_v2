#ifndef FRONT_H
#define FRONT_H

/* Define the characters used to separate strings */
#define SPACES  " \t\v\f"

/* Maximum length for a label name */
#define MAX_LABEL_LENGTH 31

/* Maximum length for a single line of assembly code */
#define MAX_LINE_LENGTH 81

/* Maximum number of variables that can be handled */
#define MAX_VARIABLES 100

/* Maximum number of labels that can be handled */
#define MAX_LABELS 100

/* Starting value for the addresses assigned to labels */
#define LABEL_START_VALUE 100

/* Struct for representing an opcode */
/* Contains the opcode name, its unique numeric code, and the number of operands it requires */
struct opcode {
    char *name;         /* Opcode name */
    int code;          /* Unique code for the opcode */
    int num_operands;  /* Number of operands required by the opcode */
};

/* Struct for representing a variable */
/* Contains the variable name and its assigned memory address */
struct variable {
    char name[32];     /* Variable name */
    int address;       /* Memory address of the variable */
};

/* Struct for representing a label */
/* Contains the label name and its assigned value */
struct label_entry {
    char name[32];     /* Label name */
    int value;         /* Value assigned to the label (used as address) */
};

/* Array to store label entries */
extern struct label_entry label_table[MAX_LABELS];
extern int label_count;  /* Number of labels currently in the table */

/* External declarations for global tables and arrays */
extern struct opcode opcode_table[16];  /* Table of opcodes */
extern char *instructions_table[];      /* List of assembler directives */
extern char *registers[];               /* List of register names */

/* Struct for storing the result of string separation */
/* Contains an array of separated strings and the count of these strings */
struct string_sep_result {
    char *strings[80];   /* Array to hold the separated strings */
    int strings_count;   /* Number of strings separated */
};

/* Function declarations for processing assembly code */

/* Separates a string by spaces and returns a result structure with separated strings */
struct string_sep_result string_sep(char *str);

/* Checks if a string matches one of the predefined assembler instructions */
/* Returns the index of the instruction if found, -1 otherwise */
int is_instruction(char *str);

/* Checks if a string matches one of the predefined opcodes */
/* Returns the index of the opcode if found, -1 otherwise */
int is_opcode(char *str);

/* Checks if a string matches one of the predefined registers */
/* Returns the index of the register if found, -1 otherwise */
int is_reg(char *str);

/* Checks if there is any extra text after the expected tokens in a line */
/* Returns 1 if extra text is found, 0 otherwise */
int extra_text(void);

/* Checks if a string is a legal label (ends with a colon) */
/* Returns 1 if the label is legal, 0 otherwise */
int check_legal_label(char *str);

/* Checks if a string matches a label */
/* Returns the label's value if it is a valid label, -1 otherwise */
int is_label(char *str);

/* Checks if a string matches a macro */
/* Returns 1 if the string is a macro, 0 otherwise */
int is_macro(const char *str);

/* Finds the address of a variable by its name */
/* Returns the variable's address if found, -1 otherwise */
int find_variable(const char *name);

/* Adds a new variable and assigns it a memory address */
/* Returns the assigned address of the new variable */
int add_variable(const char *name);

#endif /* FRONT_H */