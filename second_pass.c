#include "first_pass.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int mark_symbol_as_entry(const char* symbol);
int encode_operands(const char* opcode, const char* operands, int IC);
void build_output_files(void);

void execute_second_pass(char **lines) {
    int IC = 0;
    int i = 0;
    char label[MAX_LABEL_LEN];
    char opcode[MAX_OPCODE_LENGTH];
    char operands[MAX_OPERAND_LENGTH];
    int errors_found = 0;
    int L;

    while (lines[i] != NULL) {
        if (is_empty_or_whitespace(lines[i])) {
            i++;
            continue;
        }

        parse_line(lines[i], label, opcode, operands);

        /* Step 3: If the first field is a symbol, skip it */
        /* (This is handled by parse_line) */

        /* Step 4: Check for extern, .string, or .data */
        if (strcmp(opcode, ".extern") == 0 || strcmp(opcode, ".string") == 0 || strcmp(opcode, ".data") == 0) {
            i++;
            continue;
        }

        /* Step 5 & 6: Check for .entry directive */
        if (strcmp(opcode, ".entry") == 0) {
            if (!mark_symbol_as_entry(operands)) {
                fprintf(stderr, "Error: Symbol %s not found for .entry directive\n", operands);
                errors_found = 1;
            }
            i++;
            continue;
        }

        /* Step 7: Complete the encoding of the operands */
        if (is_opcode(opcode) != -1) {
            L = encode_operands(opcode, operands, IC);
            if (L < 0) {
                errors_found = 1;
                /* Error handling is done inside encode_operands */
            }
            /* Step 8: Update IC */
            IC += L;
        }

        i++;
    }

    /* Step 9: Check for errors */
    if (errors_found) {
        fprintf(stderr, "Errors found during second pass. Stopping.\n");
        return;
    }

    /* Step 10: Build the output files */
    build_output_files();
}

int mark_symbol_as_entry(const char* symbol) {
    /* Implement this function to mark the symbol as entry in your symbol table */
    /* Return 1 if successful, 0 if symbol not found */
    /* This is a placeholder implementation */
    return 1;
}

int encode_operands(const char* opcode, const char* operands, int IC) {
    /* Implement this function to encode the operands */
    /* Return the number of words encoded, or -1 if an error occurred */
    /* This is a placeholder implementation */
    return 1;
}

void build_output_files(void) {
    /* Implement this function to build your output files */
    /* This is a placeholder implementation */
    printf("Building output files...\n");
}

int main() {
    char** lines;
    int num_lines;
    int i;

    lines = read_asm_file("input.asm", &num_lines);
    if (lines == NULL) {
        fprintf(stderr, "Error reading assembly file\n");
        return 1;
    }

    execute_first_pass(lines);

    execute_second_pass(lines);

    /* Print labels (if needed) */
    for (i = 0; i < labels_count; i++) {
        printf("Label: %s, Address: %d\n", labels_table[i].name, labels_table[i].address);
    }

    /* Print assembly lines (if needed) */
    for (i = 0; i < num_lines; i++) {
        printf("%s\n", lines[i]);
    }

    /* Free allocated memory */
    free_lines(lines);

    return 0;
}