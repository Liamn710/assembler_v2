#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
    char label[50];
    int address;
} SymbolTableEntry;

SymbolTableEntry symbolTable[100];
int symbolCount = 0;
int currentAddress = 0;

void addLabel(char *label) {
    int i;
    for (i = 0; i < symbolCount; i++) {
        if (strcmp(symbolTable[i].label, label) == 0) {
            printf("Error: Duplicate label found: %s\n", label);
            return;
        }
    }
    strcpy(symbolTable[symbolCount].label, label);
    symbolTable[symbolCount].address = currentAddress;
    symbolCount++;
}

void processLine(char *line) {
    int j;
    int s;
    char *label;
    for (j=0; line[j] != '\0'; j++) {
        if (line[j] == ':') {
            label = malloc(j+1);
            for (s=0; s < j; s++) {
                label[s] = line[s];
            }
        }
    }

    for (s=0; s < 100; s++) {
        if (symbolTable[s].label != NULL) {
            printf("%s\n", symbolTable[s].label);
        }
    }
    addLabel(label);
    label[0] = '\0';

    currentAddress++;
}


int main(void) {
    int i;
    char *lines[] = {
        "START: MOV A, B\0",
        "        ADD A, 1\0",
        "LOOP:   SUB A, 1\0",
        "        JNZ LOOP\0",
        "        HLT\0",
        NULL
    };

    printf("hi");
    
    for (i = 0; lines[i] != NULL; i++) {
        processLine(lines[i]);
    }

    printf("Symbol Table:\n");
    for (i = 0; i < symbolCount; i++) {
        printf("%s: %d\n", symbolTable[i].label, symbolTable[i].address);
    }

    return 0;
}

