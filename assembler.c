#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "first_pass.h"
#include "second_pass.h"
#include "macro_handle.h"
#include "assembler.h"

#define MEMORY_SIZE 4096
#define MAX_FILENAME 256


int main(int argc, char *argv[]) {
    char input_filename[MAX_FILENAME];
    char preprocessed_filename[MAX_FILENAME];
    char output_filename[MAX_FILENAME];
    char **lines;
    int num_lines;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename without .as extension>\n", argv[0]);
        return 1;
    }

    /* Construct input filename */
    strcpy(input_filename, argv[1]);
    strcat(input_filename, ".as");
    
    /* Construct preprocessed filename */
    strcpy(preprocessed_filename, argv[1]);
    strcat(preprocessed_filename, "_preprocessed.as");

    /* Preprocess the assembly file */
    preprocess_assembly(input_filename, preprocessed_filename);

    /* Read preprocessed assembly file */
    lines = read_asm_file(preprocessed_filename, &num_lines);
    if (lines == NULL) {
        fprintf(stderr, "Error reading preprocessed file %s\n", preprocessed_filename);
        return 1;
    }

    /* Execute first and second pass */
    execute_first_pass(lines);
    execute_second_pass(lines);

    /* Generate output files */
    strcpy(output_filename, argv[1]);
    strcat(output_filename, ".ob");
    generate_ob_file(output_filename);

    if (has_extern_instructions()) {
        strcpy(output_filename, argv[1]);
        strcat(output_filename, ".ext");
        generate_externals_file(output_filename);
    }

    if (has_entry_instructions()) {
        strcpy(output_filename, argv[1]);
        strcat(output_filename, ".ent");
        generate_entries_file(output_filename);
    }

    /* Free allocated memory */
    free_lines(lines);

    /* Remove the preprocessed file if you don't want to keep it */
    remove(preprocessed_filename);

    return 0;
}