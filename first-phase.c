#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_LABELS 100 /* todo - maybe explicit size? */
#define MAX_LABEL_LEN 50 /* todo - maybe explicit size? */
#define START_ADDRESS 100

typedef struct {
    char *name;
    int numOfCells;
} Command;

Command commands[] = {
        {"mov",  3},
        {"cmp",  3},
        {"add",  3},
        {"sub",  2},
        {"lea",  3},
        {"clr",  2},
        {"not",  2},
        {"inc",  2},
        {"dec",  2},
        {"jmp",  2},
        {"bne",  2},
        {"red",  2},
        {"prn",  2},
        {"jsr",  2},
        {"rts",  1},
        {"stop", 1}
};

typedef struct {
    char name[MAX_LABEL_LEN]; /* todo - maybe explicit size? */
    int address;
} labelEntry;

labelEntry labelsTable[MAX_LABELS]; /* todo - maybe explicit size? */
int labelsCount = 0;
int IC = START_ADDRESS;
int DC = 0;

void addLabel(char* label, int address) {
    if (labelsCount >= MAX_LABELS) {
        printf("Error: Too many labels\n"); /* todo - error handling */
        return;
    }
    strcpy(labelsTable[labelsCount].name, label);
    labelsTable[labelsCount].address = address;
    labelsCount++;
}

int parseLabelUsingStrstr(char* line, char* label) {
    char* colonPos = strstr(line, ":");
    
    if (colonPos) {
        int labelLength = colonPos - line;
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

int calculateDataCells(char* line) {
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

int calculateStringCells(char* line) {
    int i = 0;
    int cells = 0;
    int inString = 0;

    while (line[i] != '\0') {
        if (line[i] == '"') {
            if (inString) {
                inString = 0;
                cells++;
                break;
            } else {
                inString = 1;
            }
        } else if (inString) {
            cells++;
        }
        i++;
    }

    return cells;
}

int getNumOfCellsFromCommand(char *commandName) {
    int i;
    for (i = 0; i < sizeof(commands); i++) {
            if (strcmp(commands[i].name, commandName) == 0) {
                return commands[i].numOfCells;
            }
        }
    return 0;
}

int calculateCommandCells(char* command) {
    char* operandsStart = strchr(command, ' ');
    char commandName[10] = {0};

    if (operandsStart) {
        strncpy(commandName, command, operandsStart - command);
        return getNumOfCellsFromCommand(commandName);
    }

    return 1;
}

void processLine(char* line) {
    char label[MAX_LABEL_LEN + 1] = {0};
    
    if (parseLabelUsingStrstr(line, label)) {
        char* commandStart = strstr(line, ":") + 1;
        while (isspace(*commandStart)) {
            commandStart++;
        }

        if (strncmp(commandStart, ".data", 5) == 0) {
            int cellsNeeded = calculateDataCells(commandStart + 5);
            addLabel(label, IC + DC);
            DC += cellsNeeded;
        } else if (strncmp(commandStart, ".string", 7) == 0) {
            int cellsNeeded = calculateStringCells(commandStart + 7);
            addLabel(label, IC + DC);
            DC += cellsNeeded;
        } else {
            int cellsNeeded = calculateCommandCells(commandStart);
            addLabel(label, IC);
            IC += cellsNeeded;
        }
    } else {
        if (strncmp(line, ".data", 5) == 0) {
            DC += calculateDataCells(line + 5);
        } else if (strncmp(line, ".string", 7) == 0) {
            DC += calculateStringCells(line + 7);
        } else {
            IC += calculateCommandCells(line);
        }
    }
}


void executeFirstPhase(char **lines) {
    int i;
    i = 0;
    while (lines[i] != NULL) {
        processLine(lines[i]);
        i++;
    }
}

int main(void) {
    int i;
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
        "STR: .string \"abcd\"\0",
        "LIST: .data 6, -9\0",
        ".data -100\0",
        "K: .data 31\0",
        NULL
    };
    
    executeFirstPhase(lines);

    for (i = 0; i < labelsCount; i++) {
        printf("Label: %s, Address: %d\n", labelsTable[i].name, labelsTable[i].address);
    }

    return 0;
}