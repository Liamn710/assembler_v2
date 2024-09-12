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

label_entry labels_table[MAX_LABELS];
int labels_count = 0;
int IC = START_ADDRESS;
int DC = 0;



void add_label(char* label, int address, int is_data, int is_string) {
    if (labels_count >= MAX_LABELS) {
        printf("Error: Too many labels\n"); /* todo - error handling */
        return;
    }
    strcpy(labels_table[labels_count].name, label);
    labels_table[labels_count].address = address;
    labels_table[labels_count].is_data = is_data;
    labels_table[labels_count].is_string = is_string;
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

void process_line(char* line) {
    char label[MAX_LABEL_LEN + 1] = {0};
    char* instruction;
    
    if (parse_label_using_str(line, label)) {
        instruction = strstr(line, ":") + 1;
        while (isspace(*instruction)) instruction++;

        if (strncmp(instruction, ".data", 5) == 0) {
            add_label(label, DC, 1, 0);  /* is_data = 1, is_string = 0 */
            DC += calculate_data_cells(instruction + 5);
        } else if (strncmp(instruction, ".string", 7) == 0) {
            add_label(label, DC, 0, 1);  /* is_data = 0, is_string = 1 */
            DC += calculate_string_cells(instruction + 7);
        } else {
            add_label(label, IC, 0, 0);  /* Regular label */
            IC += calculate_opcode_cells(instruction);
        }
    } else {
        /* Process line without label */
        if (strncmp(line, ".data", 5) == 0) {
            DC += calculate_data_cells(line + 5);
        } else if (strncmp(line, ".string", 7) == 0) {
            DC += calculate_string_cells(line + 7);
        } else {
            IC += calculate_opcode_cells(line);
        }
    }
}
int is_empty_or_whitespace(const char* str) {

    while (*str != '\0') {
        if (!isspace((unsigned char)*str)) {
            return 0;  /* Found a non-whitespace character */
        }
        str++;
    }
    return 1;  /* String is empty or contains only whitespace */
}

int calculate_data_cells(char* line) {
    int cells = 0;
    char* token = strtok(line, " ,\t");
    while (token != NULL) {
        cells++;
        token = strtok(NULL, " ,\t");
    }
    return cells;
}

int calculate_string_cells(char* line) {
    int cells = 0;
    char* start = strchr(line, '"');
    char* end = strrchr(line, '"');
    if (start && end && start != end) {
        cells = end - start - 1 + 1;  /* -1 to exclude quotes, +1 for null terminator */
    }
    return cells;
}

int calculate_opcode_cells(char* opcode) {
    /* This function should return the number of words used by the opcode */
    /* You might need to implement a more sophisticated logic based on your instruction set */
    return 1;  /* Assuming each opcode uses at least one word */
}
void execute_first_pass(char **lines) {
    char label[MAX_LABEL_LEN];
    char opcode[MAX_OPCODE_LENGTH];
    char operands[MAX_OPERAND_LENGTH];
    unsigned int word;
    char* binary_repr;
    int i = 0;

    while (lines[i] != NULL) {
        if (is_empty_or_whitespace(lines[i])) {
            i++;
            continue;
        }

        parse_line(lines[i], label, opcode, operands);
        
        if (label[0] != '\0') {
        int is_data = strcmp(opcode, ".data") == 0;
        int is_string = strcmp(opcode, ".string") == 0;
        add_label(label, (is_data || is_string) ? DC : IC, is_data, is_string);
    }
        if (is_instruction(opcode) != -1) {
            /* Handle special instructions */
            if (strcmp(opcode, ".data") == 0) {
                DC += count_data_elements(operands);
            } else if (strcmp(opcode, ".string") == 0) {
                DC += strlen(operands) - 2 + 1; /* -2 for quotes, +1 for null terminator */
            } else if (strcmp(opcode, ".extern") == 0) {
                add_external_label(operands);
            }
            /* No output for instructions in first pass */
        } else if (is_opcode(opcode) != -1) {
            /* Handle regular opcodes */
            word = create_first_word(opcode, operands);
            binary_repr = word_to_binary(word);
            free_binary(binary_repr);
            IC += 1 + count_additional_words(opcode, operands);
        } else if (opcode[0] != '\0') {
            fprintf(stderr, "Error: Invalid opcode or instruction '%s'\n", opcode);
        }
        i++;
    }
}
int count_additional_words(const char* opcode, const char* operands) {
    int count = 0;
    char* first_operand;
    char* second_operand;
    char operands_copy[MAX_OPERAND_LENGTH];

    strcpy(operands_copy, operands);
    first_operand = strtok(operands_copy, ",");
    second_operand = strtok(NULL, ",");

    if (first_operand != NULL) {
        count++;
    }
    if (second_operand != NULL) {
        count++;
    }

    return count;
}

int count_data_elements(const char* operands) {
    int count = 0;
    const char* ptr = operands;
    
    while (*ptr != '\0') {
        /* Skip whitespace and commas */
        while (*ptr == ' ' || *ptr == '\t' || *ptr == ',') {
            ptr++;
        }
        
        /* If we've reached a number, count it and move to the next */
        if (*ptr == '-' || (*ptr >= '0' && *ptr <= '9')) {
            count++;
            /* Move past the number */
            while (*ptr != ',' && *ptr != '\0') {
                ptr++;
            }
        } else if (*ptr != '\0') {
            /* If we're here, we've encountered an unexpected character */
            fprintf(stderr, "Error: Invalid character in .data operands\n");
            return -1;
        }
    }
    
    return count;
}

void add_external_label(const char* label) {
    if (labels_count < MAX_LABELS) {
        strncpy(labels_table[labels_count].name, label, MAX_LABEL_LEN - 1);
        labels_table[labels_count].name[MAX_LABEL_LEN - 1] = '\0';
        labels_table[labels_count].address = -1; /* Use -1 to indicate external labels */
        labels_count++;
    } else {
        fprintf(stderr, "Error: Too many labels, can't add external label %s\n", label);
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
unsigned int get_addressing_method(char* operand) {
    char trimmed_operand[MAX_OPERAND_LENGTH];

    if (operand == NULL || *operand == '\0') {
        return 0;
    }

    /* Trim the operand */
    strncpy(trimmed_operand, operand, MAX_OPERAND_LENGTH - 1);
    trimmed_operand[MAX_OPERAND_LENGTH - 1] = '\0';
    trim(trimmed_operand);

    if (trimmed_operand[0] == '#') {
        return 1; /* Immediate addressing */
    }
    if (trimmed_operand[0] == '*') {
        return 2; /* Register indirect addressing */
    }
    if (trimmed_operand[0] == 'r' && isdigit(trimmed_operand[1])) {
        return 4; /* Direct register addressing */
    }
    return 8; /* Direct addressing (label) */
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
/* Function to check if a string is an instruction */
/* Returns the index of the instruction if found, -1 otherwise */
int is_instruction(char *str) {
    const char *instructions[] = {".data", ".string", ".extern", ".entry"};
    int num_instructions = sizeof(instructions) / sizeof(instructions[0]);
    int i;

    if (str == NULL) {
        return -1;  /* Return -1 if the input string is NULL */
    }
    for (i = 0; i < num_instructions; i++) {
        if (strcmp(str, instructions[i]) == 0) {
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
unsigned int create_first_word(char* opcode, char* operands)
{
    unsigned int word = 0;
    int op_code;
    unsigned int first_addressing = 0;
    unsigned int second_addressing = 0;
    char* first_operand = NULL;
    char* second_operand = NULL;
    char* comma;

    /* Handle .data instruction */
    if (strcmp(opcode, ".data") == 0) {
        /* For .data, we don't create a first word */
        return 0;
    }

    /* Get opcode */
    op_code = is_opcode(opcode);
    if (op_code == -1) {
        /* Handle error: invalid opcode */
        fprintf(stderr, "Error: Invalid opcode '%s'\n", opcode);
        return 0;
    }

    /* Set opcode (bits 14-11) */
    word |= (op_code & 0xF) << 11;

    /* Parse operands */
    if (operands[0] != '\0') {
        first_operand = operands;
        comma = strchr(operands, ',');
        if (comma != NULL) {
            *comma = '\0';
            second_operand = comma + 1;
            trim(second_operand);
        }
        trim(first_operand);
    }

    /* Handle addressing methods */
    first_addressing = get_addressing_method(first_operand);
    second_addressing = get_addressing_method(second_operand);

    if (first_addressing != 0 && second_addressing != 0) {
        /* Two operand instruction */
        word |= (first_addressing & 0xF) << 7;  /* Set source operand addressing method (bits 10-7) */
        word |= (second_addressing & 0xF) << 3; /* Set target operand addressing method (bits 6-3) */
    } else if (first_addressing != 0) {
        /* Single operand instruction */
        word |= (first_addressing & 0xF) << 3;  /* Set target operand addressing method (bits 6-3) */
    }

    /* Set ARE field (bits 2-0) */
    /* For now, we'll set it to 100 (Absolute) for all non-data instructions */
    word |= 0x4;

    return word;
}

char* word_to_binary(unsigned int word) {
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

    /* Ensure we're only using the lowest 15 bits */
    word &= 0x7FFF;  /* 0x7FFF is 15 ones in binary */

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

/* Function to trim whitespace from both ends of a string */
void trim(char* str) {
    char* end;
    
    /* Trim leading space */
    while(isspace((unsigned char)*str)) str++;

    if(*str == 0)  /* All spaces? */
        return;

    /* Trim trailing space */
    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;

    /* Write new null terminator */
    *(end+1) = 0;
}
void parse_line(char* line, char* label, char* opcode, char* operands) {
    char* token;
    char line_copy[MAX_LINE_LENGTH];

    /* Initialize output strings */
    label[0] = '\0';
    opcode[0] = '\0';
    operands[0] = '\0';

    /* Create a copy of the line to avoid modifying the original */
    strncpy(line_copy, line, MAX_LINE_LENGTH - 1);
    line_copy[MAX_LINE_LENGTH - 1] = '\0';

    /* Check for label */
    token = strtok(line_copy, " \t");
    if (token != NULL && strchr(token, ':') != NULL) {
        /* It's a label */
        strncpy(label, token, MAX_LABEL_LEN - 1);
        label[MAX_LABEL_LEN - 1] = '\0';
        *strchr(label, ':') = '\0';  /* Remove the colon */
        token = strtok(NULL, " \t");  /* Move to the next token */
    }

    /* Check for instruction or opcode */
    if (token != NULL) {
        strncpy(opcode, token, MAX_OPCODE_LENGTH - 1);
        opcode[MAX_OPCODE_LENGTH - 1] = '\0';

        /* Get the rest of the line as operands */
        token = strtok(NULL, "\0");
        if (token != NULL) {
            strncpy(operands, token, MAX_OPERAND_LENGTH - 1);
            operands[MAX_OPERAND_LENGTH - 1] = '\0';
            trim(operands);
        }
    }
}

int* parse_data_operands(char* operands, int* count) {
    char* token;
    int* values = NULL;
    int capacity = 10;  
    int* temp;

    *count = 0;

    values = (int*)malloc(capacity * sizeof(int));
    if (values == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }

    token = strtok(operands, ",");
    while (token != NULL) {
        /* Skip leading whitespace */
        while (isspace(*token)) token++;
        
        /* Check if we need to resize the array */
        if (*count >= capacity) {
            capacity *= 2;
            temp = (int*)realloc(values, capacity * sizeof(int));
            if (temp == NULL) {
                fprintf(stderr, "Memory reallocation failed\n");
                free(values);
                return NULL;
            }
            values = temp;
        }

        /* Convert the token to an integer and add it to the array */
        values[*count] = atoi(token);
        (*count)++;

        /* Get the next token */
        token = strtok(NULL, ",");
    }

    return values;
}