#include "macro_handle.h"
#include <stdlib.h>
#include <string.h>

/* Global variables */
Macro* macro_list = NULL;

/* List of reserved words (opcodes, instructions, etc.) */
const char* reserved_words[] = {
    "MOV", "ADD", "SUB", "MUL", "DIV", "AND", "OR", "XOR", "NOT",
    "JMP", "JE", "JNE", "JG", "JGE", "JL", "JLE",
    "PUSH", "POP", "CALL", "RET",
    /* Add more reserved words as needed */
    NULL  /* Sentinel value to mark the end of the array */
};

/* Function to check if a string is a valid identifier */
int is_valid_identifier(const char* name) {
    const char* p;
    
    if (name == NULL || *name == '\0' || strlen(name) > MAX_IDENTIFIER_LENGTH) {
        return 0;  /* False */
    }
    
    if (!isalpha((unsigned char)*name) && *name != '_') {
        return 0;  /* First character must be a letter or underscore */
    }
    
    for (p = name + 1; *p != '\0'; p++) {
        if (!isalnum((unsigned char)*p) && *p != '_') {
            return 0;  /* Subsequent characters must be alphanumeric or underscore */
        }
    }
    
    return 1;  /* True */
}

/* Function to check if a string is a reserved word */
int is_reserved_word(const char* name) {
    int i;
    for (i = 0; reserved_words[i] != NULL; i++) {
        if (strcmp(name, reserved_words[i]) == 0) {
            return 1;  /* True */
        }
    }
    return 0;  /* False */
}

/* Function to check if a macro name already exists */
int macro_exists(const char* name) {
    Macro* current = macro_list;
    while (current) {
        if (strcmp(current->name, name) == 0) {
            return 1;  /* True */
        }
        current = current->next;
    }
    return 0;  /* False */
}
/* Main preprocessing function */
void preprocess_assembly(const char* input_filename, const char* output_filename) {
    FILE* input;
    FILE* output;

    input = fopen(input_filename, "r");
    output = fopen(output_filename, "w");
    
    if (input == NULL || output == NULL) {
        printf("Error opening files.\n");
        return;
    }
    
    expand_macros(input, output);
    
    fclose(input);
    fclose(output);
    
    free_macros(macro_list);
}

/* Function to expand macros in the input file */
void expand_macros(FILE* input, FILE* output) {
    char line[MAX_LINE_LENGTH];
    size_t len;
    int is_macro;
    Macro* current;
    Macro* new_macro;
    char macro_name[MAX_LINE_LENGTH];
    
    while (fgets(line, sizeof(line), input) != NULL) {
        /* Remove newline character if present */
        len = strlen(line);
        if (len > 0 && line[len-1] == '\n') {
            line[len-1] = '\0';
        }
        
        if (strncmp(line, "macr", 4) == 0) {
            /* Define a new macro */
            new_macro = define_macro(input, line);
            if (new_macro) {
                new_macro->next = macro_list;
                macro_list = new_macro;
            }
        } else {
            /* Check if the line is a macro call */
            is_macro = 0;
            current = macro_list;
            while (current) {
                if (sscanf(line, "%s", macro_name) == 1 && strcmp(macro_name, current->name) == 0) {
                    expand_macro(output, current);
                    is_macro = 1;
                    break;
                }
                current = current->next;
            }
            
            /* If not a macro call, write the line as is */
            if (!is_macro) {
                fprintf(output, "%s\n", line);
            }
        }
    }
}

/* Function to define a new macro */
Macro* define_macro(FILE* input, const char* macro_line) {
    char macro_name[MAX_IDENTIFIER_LENGTH + 1];
    Macro* macro;
    MacroLine* last_line = NULL;
    char line[MAX_LINE_LENGTH];
    size_t len;
    MacroLine* new_line;

    /* Check for exact "macr NAME" format */
    if (sscanf(macro_line, "macr %s%*[^\n]", macro_name) != 1) {
        printf("Error: Invalid macro declaration format. Use 'macr NAME' without any extra text.\n");
        return NULL;
    }

    /* Validate macro name */
    if (!is_valid_identifier(macro_name)) {
        printf("Error: Invalid macro name '%s'. Must be a valid identifier.\n", macro_name);
        return NULL;
    }

    if (is_reserved_word(macro_name)) {
        printf("Error: Macro name '%s' is a reserved word.\n", macro_name);
        return NULL;
    }

    if (macro_exists(macro_name)) {
        printf("Error: Macro '%s' is already defined.\n", macro_name);
        return NULL;
    }
    
    macro = (Macro*)malloc(sizeof(Macro));
    if (macro == NULL) {
        printf("Error: Memory allocation failed.\n");
        return NULL;
    }

    macro->name = my_strdup(macro_name);
    macro->lines = NULL;
    macro->next = NULL;
    
    /* Read macro lines */
    while (fgets(line, sizeof(line), input) != NULL) {
        len = strlen(line);
        if (len > 0 && line[len-1] == '\n') {
            line[len-1] = '\0';
        }
        
        /* Trim leading whitespace */
        {
            char* trimmed_line = line;
            while (*trimmed_line == ' ' || *trimmed_line == '\t') {
                trimmed_line++;
            }
            
            if (strcmp(trimmed_line, "endmacr") == 0) {
                return macro;  /* Macro definition complete */
            }
            
            new_line = (MacroLine*)malloc(sizeof(MacroLine));
            if (new_line == NULL) {
                printf("Error: Memory allocation failed.\n");
                free_macros(macro);
                return NULL;
            }

            new_line->content = my_strdup(trimmed_line);
            new_line->next = NULL;
            
            if (last_line == NULL) {
                macro->lines = new_line;
            } else {
                last_line->next = new_line;
            }
            last_line = new_line;
        }
    }
    
    printf("Error: Unexpected end of file while defining macro '%s'.\n", macro->name);
    free_macros(macro);
    return NULL;
}
/* Function to expand a macro */
void expand_macro(FILE* output, Macro* macro) {
    MacroLine* current = macro->lines;
    while (current) {
        fprintf(output, "%s\n", current->content);
        current = current->next;
    }
}

/* Function to free all allocated memory for macros */
void free_macros(Macro* macro_list) {
    Macro* next_macro;
    MacroLine* current_line;
    MacroLine* next_line;

    while (macro_list) {
        next_macro = macro_list->next;
        current_line = macro_list->lines;
        while (current_line) {
            next_line = current_line->next;
            free(current_line->content);
            free(current_line);
            current_line = next_line;
        }
        free(macro_list->name);
        free(macro_list);
        macro_list = next_macro;
    }
}

/* Custom strdup function since C90 doesn't include it */
char* my_strdup(const char* s) {
    size_t len = strlen(s) + 1;
    char* new_str = (char*)malloc(len);
    if (new_str == NULL) {
        return NULL;
    }
    return memcpy(new_str, s, len);
}