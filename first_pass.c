#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "first_pass.h"



/* Opcode table initialization */
/* Each entry contains the opcode name, its unique number, and the number of operands */
struct opcode opcode_table[16] = {
    {"mov", 2}, {"cmp", 2}, {"add", 2}, {"sub", 2},
    {"lea", 2}, {"clr", 2}, {"not", 1}, {"inc", 1},
    {"dec", 1}, {"jmp", 1}, {"bne", 2}, {"red", 1},
    {"prn", 1}, {"jsr", 1}, {"rts", 1}, {"stop", 0}
};

/* Instruction table */
/* Contains assembler directives like .data, .string, etc. */
char *instructions_table[] = {".data", ".string", ".extern", ".entry"};

/* Registers */
/* Contains the register names r0 through r7 */
char *registers[] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"};

typedef struct {
    char name[MAX_LABEL_LEN]; /* todo - maybe explicit size? */
    int address;
} label_entry;

label_entry labels_table[MAX_LABELS]; /* todo - maybe explicit size? */
int labels_count = 0;
int IC = START_ADDRESS;
int DC = 0;

void add_label(char* label, int address) {
    if (labels_count >= MAX_LABELS) {
        printf("Error: Too many labels\n"); /* todo - error handling */
        return;
    }
    strcpy(labels_table[labels_count].name, label);
    labels_table[labels_count].address = address;
    labels_count++;
}

int parse_label_using_str(char* line, char* label) {
    char* colon_pos = strstr(line, ":");
    
    if (colon_pos) {
        int labelLength = colon_pos - line;
        if (labelLength <= MAX_LABEL_LEN) {
            strncpy(label, line, labelLength);
            label[labelLength] = '\0';
            return 1;
        } else {
            printf("Error: Label too long\n"); /* todo - error handling */
        }
    }

    return 0;
}

int calculate_data_cells(char* line) {
    int i = 0;
    int cells = 0;
    
    while (line[i] != '\0') {
        while (isspace(line[i]) || line[i] == ',') {
            i++;
        }
        if (line[i] == '-' || isdigit(line[i])) {
            cells++;

            while (line[i] != '\0' && line[i] != ',' && !isspace(line[i])) {
                i++;
            }
        }
    }

    return cells;
}

int calculate_string_cells(char* line) {
    int i = 0;
    int cells = 0;
    int in_string = 0;

    while (line[i] != '\0') {
        if (line[i] == '"') {
            if (in_string) {
                in_string = 0;
                cells++;
                break;
            } else {
                in_string = 1;
            }
        } else if (in_string) {
            cells++;
        }
        i++;
    }

    return cells;
}

int get_num_of_cells_from_opcode(char *opcode_name) {
    int i;
    int num_opcodes;
    num_opcodes = sizeof(opcode_table) / sizeof(opcode_table[0]);
    for (i = 0; i < num_opcodes; i++) {
        if (strcmp(opcode_table[i].name, opcode_name) == 0) {
            return opcode_table[i].num_operands + 1;
        }
    }
    return 0;
}

int calculate_opcode_cells(char* opcode) {
    char* operands_start = strchr(opcode, ' ');
    char opcode_name[10] = {0};

    if (operands_start) {
        strncpy(opcode_name, opcode, operands_start - opcode);
        return get_num_of_cells_from_opcode(opcode_name);
    }

    return 1;
}

void process_line(char* line) {
    char label[MAX_LABEL_LEN + 1] = {0};
    
    if (parse_label_using_str(line, label)) {
        char* opcode_start = strstr(line, ":") + 1;
        while (isspace(*opcode_start)) {
            opcode_start++;
        }

        if (strncmp(opcode_start, ".data", 5) == 0) {
            int cells_needed = calculate_data_cells(opcode_start + 5);
            add_label(label, IC + DC);
            DC += cells_needed;
        } else if (strncmp(opcode_start, ".string", 7) == 0) {
            int cells_needed = calculate_string_cells(opcode_start + 7);
            add_label(label, IC + DC);
            DC += cells_needed;
        } else {
            int cells_needed = calculate_opcode_cells(opcode_start);
            add_label(label, IC);
            IC += cells_needed;
        }
    } else {
        if (strncmp(line, ".data", 5) == 0) {
            DC += calculate_data_cells(line + 5);
        } else if (strncmp(line, ".string", 7) == 0) {
            DC += calculate_string_cells(line + 7);
        } else {
            IC += calculate_opcode_cells(line);
        }
    }
}


void execute_first_pass(char **lines) {
    int i;
    i = 0;
    while (lines[i] != NULL) {
        process_line(lines[i]);
        i++;
    }
}



char** read_asm_file(const char* filename, int* num_lines) {
    FILE* file;
    char** lines = NULL;
    char buffer[MAX_LINE_LENGTH];
    int capacity = INITIAL_LINES;
    int line_count = 0;
    size_t len;
    int i;
    char** temp;

    /* Open the file */
    file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening file: %s\n", filename);
        return NULL;
    }

    /* Allocate initial memory for lines */
    lines = (char**)malloc(capacity * sizeof(char*));
    if (lines == NULL) {
        fclose(file);
        return NULL;
    }

    /* Read lines from the file */
    while (fgets(buffer, MAX_LINE_LENGTH, file) != NULL) {
        /* Remove newline character if present */
        len = strlen(buffer);
        if (len > 0 && buffer[len-1] == '\n') {
            buffer[len-1] = '\0';
        }

        /* Allocate memory for the line and copy it */
        lines[line_count] = (char*)malloc((len + 1) * sizeof(char));
        if (lines[line_count] == NULL) {
            /* Handle allocation failure */
            for (i = 0; i < line_count; i++) {
                free(lines[i]);
            }
            free(lines);
            fclose(file);
            return NULL;
        }
        strcpy(lines[line_count], buffer);

        line_count++;

        /* Resize the array if necessary */
        if (line_count >= capacity) {
            capacity *= 2;
            temp = (char**)realloc(lines, capacity * sizeof(char*));
            if (temp == NULL) {
                /* Handle reallocation failure */
                for (i = 0; i < line_count; i++) {
                    free(lines[i]);
                }
                free(lines);
                fclose(file);
                return NULL;
            }
            lines = temp;
        }
    }

    /* Add NULL terminator */
    lines[line_count] = NULL;

    fclose(file);
    *num_lines = line_count;
    return lines;
}

/* Function to free the memory allocated for lines */
void free_lines(char** lines) {
    int i;
    if (lines == NULL) return;

    for (i = 0; lines[i] != NULL; i++) {
        free(lines[i]);
    }
    free(lines);
}
#include <stdint.h>

/* Function prototypes for user's existing functions */


/* Function to determine the addressing method */
unsigned int get_addressing_method(char* operand)
{
    /* No operand case */
    if (operand == NULL || *operand == '\0')
    {
        return 0;
    }
    /* Immediate addressing */
    if (operand[0] == '#')
    {
        return 1;
    }
    /* Register indirect addressing */
    if (operand[0] == '*' && is_reg(operand + 1))
    {
        return 2;
    }
    /* Direct register addressing */
    if (is_reg(operand))
    {
        return 4;
    }
    /* Direct addressing (label) */
    return 8;
}

int is_opcode(char *str) {
    int i;
    if (str == NULL) {
        return -1;  /* Return -1 if the input string is NULL */
    }
    for (i = 0; i < 16; i++) {  /* Iterate over the opcode table */
        if (strcmp(str, opcode_table[i].name) == 0) {
            return i;  /* Return the index if a match is found */
        }
    }
    return -1;  /* Return -1 if no match is found */
}

int is_reg(char *str) {
    int i;
    if (str == NULL) {
        return -1;  /* Return -1 if the input string is NULL */
    }
    for (i = 0; i < 8; i++) {  /* Iterate over the register array */
        if (strcmp(str, registers[i]) == 0) {
            return i;  /* Return the index if a match is found */
        }
    }
    return -1;  /* Return -1 if no match is found */
}

/* Function to create the first word of the binary code */
unsigned int create_first_word( char* opcode, char* first_operand, char* second_operand)
{
    unsigned int word = 0;
    unsigned int op_code;
    unsigned int first_addressing = 0;
    unsigned int second_addressing = 0;

    /* Get opcode (assuming is_opcode returns the numeric value of the opcode) */
    op_code = is_opcode(opcode);

    /* Set opcode (bits 14-11) */
    word |= (op_code & 0xF) << 11;

    /* Handle addressing methods */
    if (first_operand != NULL)
    {
        first_addressing = get_addressing_method(first_operand);
        if (second_operand != NULL)
        {
            /* Two operand instruction */
            second_addressing = get_addressing_method(second_operand);
            /* Set source operand addressing method (bits 10-7) */
            word |= (first_addressing & 0xF) << 7;
            /* Set target operand addressing method (bits 6-3) */
            word |= (second_addressing & 0xF) << 3;
        }
        else
        {
            /* Single operand instruction */
            /* Set target operand addressing method (bits 6-3) */
            word |= (first_addressing & 0xF) << 3;
        }
    }

    /* Set ARE field (bits 2-0) */
    word |= 0x4; /* 100 in binary */

    return word;
}

char* word_to_binary(int word) {
    char* binary;
    int i;
    unsigned int mask;

    /* Allocate memory for the binary string (15 bits + null terminator) */
    binary = (char*)malloc(16 * sizeof(char));
    if (binary == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }

    /* Initialize the string with null terminator */
    binary[15] = '\0';

    /* Convert to 15-bit binary representation */
    mask = 1 << 14;  /* Start with the 15th bit (remember, it's 0-indexed) */
    for (i = 0; i < 15; i++) {
        binary[i] = (word & mask) ? '1' : '0';
        mask >>= 1;
    }

    return binary;
}

/* Function to free the memory allocated for the binary string */
void free_binary(char* binary) {
    free(binary);
}
int main() {
    char* binary_repr;
    unsigned int first_word;
    int num_lines;
    char** asm_lines;
    int i;

    asm_lines = read_asm_file("output.asm", &num_lines);
    execute_first_pass(asm_lines);
    for (i = 0; i < labels_count; i++) {
        printf("Label: %s, Address: %d\n", labels_table[i].name, labels_table[i].address);
    }

    if (asm_lines != NULL) {
        for (i = 0; asm_lines[i] != NULL; i++) {
            printf("%s\n", asm_lines[i]);
        }
        free_lines(asm_lines);
    }
    first_word = create_first_word("add", "r3", "#100");
    binary_repr = word_to_binary(first_word);
    if (binary_repr != NULL) {
        printf("Binary representation: %s\n", binary_repr);
        free_binary(binary_repr);
    }
    return 0;
}
