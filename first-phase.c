#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef int bool;
#define true 1
#define false 0

typedef struct {
    char label[50];
    int address;
} LabelEntry;

LabelEntry labelsTable[100];
int labelsCount = 0;
int currentAddress = 0;

bool isLabelLine(char *line) {
    int i;
    for (i=0; line[i] != '\0'; i++) {
        if (line[i] == ':') {
            return true;
        }
    }
    return false;
}

int getEndIndexOfLabel(char *line) {
    int i;

    while (line[i] != '\0') {
        if (line[i] == ':') {
            return i;
        }
        i++;
    }

    return -1;
}

char* getLabel(char *line, int endIndex) {
    int i;
    char *label = malloc(endIndex+1);

    for (i=0; i < endIndex; i++) {
        label[i] = line[i];
    }

    return label;
}

char *copy(char *destination, const char *source) {
    char *start = destination;

    while (*source != '\0') {
        *destination = *source;
        destination++;
        source++;
    }

    *destination = '\0';

    return start;
}

void addLabel(char *label) {
    int i;
    for (i = 0; i < labelsCount; i++) {
        if (strcmp(labelsTable[i].label, label) == 0) {
            printf("Error: Duplicate label found: %s\n", label);
            return;
        }
    }
    copy(labelsTable[labelsCount].label, label);
    labelsTable[labelsCount].address = currentAddress;
    labelsCount++;
}

void processLine(char *line) {
    int endIndexOfLabel;
    char *label;
    
    if (isLabelLine(line) == true) {
        endIndexOfLabel = getEndIndexOfLabel(line);
        label = getLabel(line, endIndexOfLabel);
        addLabel(label);
    }

    currentAddress++;
}

void executeFirstPhase(char **lines) {
    int currentLine;
    int i;
    
    for (currentLine = 0; lines[currentLine] != NULL; currentLine++) {
        processLine(lines[currentLine]);
    }

    printf("Labels Table:\n");
    for (i = 0; i < labelsCount; i++) {
        printf("label: %s, address: %d\n", labelsTable[i].label, labelsTable[i].address);
    }
}

int main(void) {
    char *lines[] = {
        "MAIN: add r3, LIST\0",
        "LOOP: prn #48\0",
        "lea STR, r6\0",
        "inc r6\0",
        "mov *r6,K\0",
        "sub r1, r4\0",
        "cmp r3, #-6\0",
        "bne END\0",
        "dec K\0",
        "jmp LOOP\0",
        "END: stop\0",
        "STR: .string “abcd”\0",
        "\0",
        "\0",
        "\0",
        "\0",
        "LIST: .data 6, -9\0",
        ".data -100\0",
        "K: .data 31\0",
        NULL
    };
    
    executeFirstPhase(lines);

    return 0;
}
