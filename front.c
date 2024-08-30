#include "front.h"
#include "macro_handle.h"
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* Opcode table initialization */
/* Each entry contains the opcode name, its unique number, and the number of operands */
struct opcode opcode_table[16] = {
    {"mov", 0, 2}, {"cmp", 1, 2}, {"add", 2, 2}, {"sub", 3, 2},
    {"lea", 4, 2}, {"clr", 5, 2}, {"not", 6, 1}, {"inc", 7, 1},
    {"dec", 8, 1}, {"jmp", 9, 1}, {"bne", 10, 2}, {"red", 11, 1},
    {"prn", 12, 1}, {"jsr", 13, 1}, {"rts", 14, 1}, {"stop", 15, 0}
};

/* Instruction table */
/* Contains assembler directives like .data, .string, etc. */
char *instructions_table[] = {".data", ".string", ".extern", ".entry"};
struct label_entry label_table[MAX_LABELS];
int label_count = 0;

/* Registers */
/* Contains the register names r0 through r7 */
char *registers[] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"};
int label_value_counter = LABEL_START_VALUE;  /* Counter for label values */

/* Function to separate strings by spaces */
/* Returns a struct containing separated strings */
struct string_sep_result string_sep(char *str) {
    int strings_count = 0;  /* Counter for separated strings */
    char *s;
    struct string_sep_result ssr = {0};  /* Initialize the result struct to zero */

    /* Skip initial whitespace */
    while (isspace(*str)) str++;

    /* Return if string is empty */
    if (*str == '\0') {
        return ssr;
    }

    do {
        ssr.strings[strings_count++] = str;  /* Store the start of the string */
        s = strpbrk(str, SPACES);  /* Find the next space */
        if (s) {
            *s = '\0';  /* Null-terminate the current string */
            s++;
            while (isspace(*s)) s++;  /* Skip additional spaces */
            if (*s == '\0')
                break;
            str = s;  /* Move to the next string */
        } else {
            break;
        }
    } while (1);

    ssr.strings_count = strings_count;  /* Set the count of separated strings */
    return ssr;
}

/* Function to check if a string is an instruction */
/* Returns the index of the instruction if found, -1 otherwise */
int is_instruction(char *str) {
    int i;
    if (str == NULL) {
        return -1;  /* Return -1 if the input string is NULL */
    }
    for (i = 0; i < 4; i++) {  /* Iterate over the instruction table */
        if (strcmp(str, instructions_table[i]) == 0) {
            return i;  /* Return the index if a match is found */
        }
    }
    return -1;  /* Return -1 if no match is found */
}

/* Function to check if a string is an opcode */
/* Returns the index of the opcode if found, -1 otherwise */
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

/* Function to check if a string is a register */
/* Returns the index of the register if found, -1 otherwise */
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

/* Function to check for extra text after expected tokens */
/* Returns 1 if there is extra text, 0 otherwise */
int extra_text() {
    char *token;
    token = strtok(NULL, "\n");  /* Get the remaining text after the last token */
    if (token != NULL) {
        return 1;  /* Return 1 if extra text is found */
    }
    return 0;  /* Return 0 if no extra text */
}

/* Function to check if a label is legal */
/* Returns 1 if the label is legal, 0 otherwise */
int check_legal_label(char *str) {
    if (str[strlen(str) - 1] == ':') {  /* Check if the label ends with a colon */
        str[strlen(str) - 1] = '\0';  /* Remove the colon */
        return 1;  /* Return 1 indicating a legal label */
    }
    return 0;  /* Return 0 indicating an illegal label */
}

/* Function to add a label to the label table */
void add_label(const char *label) {
    if (label_count >= MAX_LABELS) {  /* Check if the label table is full */
        fprintf(stderr, "Label table overflow.\n");
        exit(1);  /* Exit with an error if the table is full */
    }
    /* Copy the label name and assign it a value */
    strncpy(label_table[label_count].name, label, sizeof(label_table[label_count].name) - 1);
    label_table[label_count].value = label_value_counter++;
    label_count++;  /* Increment the label count */
}

/* Function to find a label's address */
/* Returns the address if found, -1 otherwise */
int find_label_address(const char *label) {
    int i;
    for (i = 0; i < label_count; i++) {  /* Iterate over the label table */
        if (strcmp(label_table[i].name, label) == 0) {
            return label_table[i].value;  /* Return the address if found */
        }
    }
    return -1;  /* Return -1 if the label is not found */
}

/* Function to check if a string is a label */
/* Returns the label's value if found, -1 otherwise */
int is_label(char *str){
    if (check_legal_label(str) > 0){  /* Check if the label is legal */
        int value;
        add_label(str);  /* Add the label to the table */
        value = find_label_address(str);  /* Find the label's address */
        if(value >= 0){
            return value;  /* Return the label's value if found */
        }
    }
    return -1;  /* Return -1 if the label is not found or illegal */
}
