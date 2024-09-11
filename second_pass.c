#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "first_pass.h"

void encode_operand(char* operand, int addressing_method, int* word_count) {
    unsigned int word = 0;
    int value;
    char* binary_repr;

    switch (addressing_method) {
        case 1: /* Immediate addressing */
            value = atoi(operand + 1); /* Skip the '#' */
            word = (value & 0x3FFF) | (A << 14);
            break;
        case 2: /* Register indirect addressing */
            if (operand[0] == '*' && is_register(operand + 1)) {
                value = get_register_number(operand + 1);
                word = (value << 7) | (A << 14); /* Put register number in bits 7-9 */
            } else {
                fprintf(stderr, "Error: Invalid register indirect addressing '%s'\n", operand);
                return;
            }
            break;
        case 4: /* Direct register addressing */
            if (is_register(operand)) {
                value = get_register_number(operand);
                word = (value << 7) | (A << 14); /* Put register number in bits 7-9 */
            } else {
                fprintf(stderr, "Error: Invalid register '%s'\n", operand);
                return;
            }
            break;
        case 8: /* Direct addressing (label) */
            remove_spaces(operand);
            value = get_label_address(operand);
            if (value != -1) {
                word = (value & 0x3FFF) | (R << 14);
            } else if (is_external_label(operand)) {
                word = (E << 14); /* Address will be filled by the linker */
                add_external_reference(operand, IC + *word_count);
            } else {
                fprintf(stderr, "Error: Undefined label '%s'\n", operand);
                return;
            }
            break;
        default:
            fprintf(stderr, "Error: Invalid addressing method\n");
            return;
    }

    binary_repr = word_to_binary(word);
    printf("%s\n", binary_repr);
    free_binary(binary_repr);
    (*word_count)++;
}
int is_register(const char* str) {
    return (str[0] == 'r' && str[1] >= '0' && str[1] <= '7' && str[2] == '\0');
}

/* Helper function to get register number */
int get_register_number(const char* reg) {
    return reg[1] - '0';
}
void remove_spaces(char* str) {
    int i, j;
    for (i = 0, j = 0; str[i]; i++) {
        if (str[i] != ' ' && str[i] != '\t') {
            str[j++] = str[i];
        }
    }
    str[j] = '\0';
}
void execute_second_pass(char **lines) {
    int i = 0;
    char label[MAX_LABEL_LEN];
    char opcode[MAX_OPCODE_LENGTH];
    char operands[MAX_OPERAND_LENGTH];
    char first_operand[MAX_OPERAND_LENGTH];
    char second_operand[MAX_OPERAND_LENGTH];
    int word_count;
    char *token;
    unsigned int first_word;

    IC = START_ADDRESS;

    while (lines[i] != NULL) {
        if (is_empty_or_whitespace(lines[i])) {
            i++;
            continue;
        }

        parse_line(lines[i], label, opcode, operands);

        if (is_instruction(opcode) != -1) {
            /* Handle directives */
            if (strcmp(opcode, ".entry") == 0) {
                trim(operands);
                mark_label_as_entry(operands);
            }
            /* Skip other directives */
        } else if (is_opcode(opcode) != -1) {
            /* Handle regular opcodes */
            word_count = 1;

            /* Store first word */
            first_word = create_first_word(opcode, operands);
            store_word(first_word);

            /* Split and trim operands */
            first_operand[0] = '\0';
            second_operand[0] = '\0';
            token = strtok(operands, ",");
            if (token != NULL) {
                strncpy(first_operand, token, MAX_OPERAND_LENGTH - 1);
                first_operand[MAX_OPERAND_LENGTH - 1] = '\0';
                trim(first_operand);
                
                token = strtok(NULL, ",");
                if (token != NULL) {
                    strncpy(second_operand, token, MAX_OPERAND_LENGTH - 1);
                    second_operand[MAX_OPERAND_LENGTH - 1] = '\0';
                    trim(second_operand);
                }
            }

            /* Encode operands */
            if (first_operand[0] != '\0') {
                encode_operand(first_operand, get_addressing_method(first_operand), &word_count);
            }
            if (second_operand[0] != '\0') {
                encode_operand(second_operand, get_addressing_method(second_operand), &word_count);
            }

            IC += word_count;
        }

        i++;
    }
}


int get_label_address(char* label) {
    int i;
    for (i = 0; i < labels_count; i++) {
        if (strcmp(labels_table[i].name, label) == 0) {
            return labels_table[i].address;
        }
    }
    return -1; /* Label not found */
}

int is_external_label(char* label) {
    int i;
    for (i = 0; i < labels_count; i++) {
        if (strcmp(labels_table[i].name, label) == 0 && labels_table[i].address == -1) {
            return 1; /* External label */
        }
    }
    return 0;
}

void add_external_reference(char* label, int address) {
    /* Implement this function to add external references for the linker */
    printf("External reference: %s at address %d\n", label, address);
}

void mark_label_as_entry(char* label) {
    /* Implement this function to mark labels as entry points */
    printf("Entry point: %s\n", label);
}

void generate_ob_file(const char* filename) {
    FILE* file;
    int address;
    unsigned int word;

    file = fopen(filename, "w");
    if (file == NULL) {
        fprintf(stderr, "Error opening file %s for writing\n", filename);
        return;
    }

    /* Write IC and DC to the first line */
    fprintf(file, "%04d %04d\n", IC - START_ADDRESS, DC);

    /* Write machine code */
    for (address = START_ADDRESS; address < START_ADDRESS + memory_used; address++) {
        word = get_memory_word(address);
        fprintf(file, "%04d %05d\n", address, word);
    }

    fclose(file);
}
/* Helper function to get a word from memory */

void generate_externals_file(const char* filename) {
    FILE* file;
    int i;

    file = fopen(filename, "w");
    if (file == NULL) {
        fprintf(stderr, "Error opening file %s for writing\n", filename);
        return;
    }

    for (i = 0; i < labels_count; i++) {
        if (labels_table[i].address == -1) {  /* Assuming -1 indicates an external label */
            /* You need to implement a way to track where this external label is used */
            /* This might involve creating a separate structure during the second pass */
            /* For now, we'll just print the label name without an address */
            fprintf(file, "%s\n", labels_table[i].name);
        }
    }

    fclose(file);
}
void store_word(unsigned int word) {
    if (memory_used < MEMORY_SIZE) {
        memory[memory_used++] = word;
    } else {
        fprintf(stderr, "Error: Memory overflow\n");
    }
}
unsigned int get_memory_word(int address) {
    if (address >= START_ADDRESS && address < START_ADDRESS + memory_used) {
        return memory[address - START_ADDRESS];
    }
    return 0; /* Return 0 for uninitialized memory */
}

void generate_entries_file(const char* filename) {
    FILE* file;
    int i;

    file = fopen(filename, "w");
    if (file == NULL) {
        fprintf(stderr, "Error opening file %s for writing\n", filename);
        return;
    }

    for (i = 0; i < labels_count; i++) {
        if (labels_table[i].is_entry) {
            fprintf(file, "%s %04d\n", labels_table[i].name, labels_table[i].address);
        }
    }

    fclose(file);
}

int has_extern_instructions() {
    int i;
    for (i = 0; i < labels_count; i++) {
        if (labels_table[i].address == -1) {  /* Assuming -1 indicates an external label */
            return 1;
        }
    }
    return 0;
}


int has_entry_instructions() {
    int i;
    for (i = 0; i < labels_count; i++) {
        if (labels_table[i].is_entry) {
            return 1;
        }
    }
    return 0;
}