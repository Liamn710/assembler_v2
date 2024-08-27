#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 81  /* 80 characters + null terminator */

/* Structure to store a single line of a macro */
typedef struct MacroLine {
    char *content;
    struct MacroLine *next;
} MacroLine;

/* Structure to store macro information */
typedef struct Macro {
    char *name;
    MacroLine *lines;
    struct Macro *next;
} Macro;

/* Function prototypes */
void preprocess_assembly(const char* input_filename, const char* output_filename);
void expand_macros(FILE* input, FILE* output);
Macro* define_macro(FILE* input, const char* macro_line);
void expand_macro(FILE* output, Macro* macro);
void free_macros(Macro* macro_list);
char* my_strdup(const char* s);

/* Global variables */
Macro* macro_list = NULL;

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
    
    while (fgets(line, sizeof(line), input) != NULL) {
        /* Remove newline character if present */
        len = strlen(line);
        if (len > 0 && line[len-1] == '\n') {
            line[len-1] = '\0';
        }
        
        if (strncmp(line, "m_macr", 6) == 0) {
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
                if (strcmp(line, current->name) == 0) {
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
    char macro_name[MAX_LINE_LENGTH];
    Macro* macro;
    MacroLine* last_line = NULL;
    char line[MAX_LINE_LENGTH];
    size_t len;
    MacroLine* new_line;

    if (sscanf(macro_line, "m_macr %s", macro_name) != 1) {
        printf("Error: Invalid macro declaration format.\n");
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
        
        if (strcmp(line, "endmacr") == 0) {
            return macro;  /* Macro definition complete */
        }
        
        new_line = (MacroLine*)malloc(sizeof(MacroLine));
        if (new_line == NULL) {
            printf("Error: Memory allocation failed.\n");
            free_macros(macro);
            return NULL;
        }

        new_line->content = my_strdup(line);
        new_line->next = NULL;
        
        if (last_line == NULL) {
            macro->lines = new_line;
        } else {
            last_line->next = new_line;
        }
        last_line = new_line;
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