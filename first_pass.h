#ifndef FIRST_PASS
#define FIRST_PASS

#define MAX_LABELS 100 
#define MAX_LABEL_LEN 50 
#define START_ADDRESS 100
#define MAX_LINE_LENGTH 100
#define INITIAL_LINES 100
#define MAX_OPCODE_LENGTH 20
#define MAX_OPERAND_LENGTH 20

struct opcode {
    char *name;         /* Opcode name */
    int num_operands;  /* Number of operands required by the opcode */
};

typedef struct {
    char name[MAX_LABEL_LEN];
    int address;
} label_entry;
extern struct opcode opcode_table[16];  /* Table of opcodes */
extern char *instructions_table[];      /* List of assembler directives */
extern char *registers[];               /* List of register names */
extern label_entry labels_table[MAX_LABELS];
extern int labels_count;
extern int IC;
extern int DC;
/* Function prototypes for user's existing functions */
int is_opcode(char *str);
int is_reg(char *str);
void parse_line(char* line, char* label, char* opcode, char* operands);
unsigned int create_first_word(char* opcode, char* operands);
char* word_to_binary(unsigned int word);
void free_binary(char* binary);
void execute_first_pass(char **lines);
char** read_asm_file(const char* filename, int* num_lines);
void free_lines(char** lines);
/* Checks if a string matches one of the predefined assembler instructions */
/* Returns the index of the instruction if found, -1 otherwise */
int is_instruction(char *str);
void trim(char* str);
int* parse_data_operands(char* operands, int* count);
int is_empty_or_whitespace(const char* str);
int count_data_elements(const char* operands);
void add_external_label(const char* label);
int count_additional_words(const char* opcode, const char* operands);

#endif