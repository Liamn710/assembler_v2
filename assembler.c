#include <stdio.h>
#include <stdlib.h>

/* Function prototype for the preprocessor */
void preprocess_assembly(const char* input_filename, const char* output_filename);
char *my_strtok(char *str, const char *delim) {
    static char *static_str = NULL;  
    char *token;  

    if (str != NULL) {
        static_str = str;
    }

    if (static_str == NULL) {
        return NULL;
    }

    static_str += strspn(static_str, delim);

    if (*static_str == '\0') {
        static_str = NULL;
        return NULL;
    }

    token = static_str;
    static_str = strpbrk(static_str, delim);

    if (static_str != NULL) {
        *static_str = '\0';
        static_str++;
    }

    return token;
}
int main(int argc, char *argv[]) {
    const char *input_filename;
    const char *output_filename;

    if (argc != 3) {
        printf("Usage: %s <input_file> <output_file>\n", argv[0]);
        return 1;
    }

    input_filename = argv[1];
    output_filename = argv[2];

    printf("Preprocessing %s to %s\n", input_filename, output_filename);
    preprocess_assembly(input_filename, output_filename);

    printf("Preprocessing complete.\n");

    return 0;
}