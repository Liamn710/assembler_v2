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

/* Example usage */
int main() {
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

    return 0;
}
