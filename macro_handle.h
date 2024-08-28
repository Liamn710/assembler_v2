#ifndef MACRO_HANDLE_H
#define MACRO_HANDLE_H

#include <stdio.h>

#define MAX_LINE_LENGTH 81  /* 80 characters + null terminator */
#define MAX_IDENTIFIER_LENGTH 31  /* Maximum length for identifiers */
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
int is_valid_identifier(const char* name);
int is_reserved_word(const char* name);

#endif /* MACRO_HANDLE_H_H */