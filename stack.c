#include "hashtable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
FileStack* initStack() {
    FileStack* s = (FileStack*)malloc(sizeof(FileStack));
    s->head = -1;
    return s;
}

bool checkEmpty(FileStack* s) {
     if(s->head == -1)
        return true;
    return false; // Added explicit false
}

bool checkFull(FileStack* s) {
    if(s->head == LIMIT_STACK - 1)
         return true;
    return false; // Added explicit false
}

void addFile(FileStack* s, const char* filename) {
    if (checkFull(s)) {
        printf("Stack full! Cannot add %s\n", filename);
        return;
    }
    s->head++;
    s->files[s->head] = strdup(filename);
}

char* removeFile(FileStack* s) {
    if (checkEmpty(s)) {
        printf("Stack empty!\n");
        return NULL;
    }
    char* current = s->files[s->head];
    s->head--;
    return current;
}

int detectThreat(const char* filename) {
    const char* blocked[] = {".exe", ".bat", ".scr"};
    int total = sizeof(blocked) / sizeof(blocked[0]);
    for (int i = 0; i < total; i++) {
        if (strstr(filename, blocked[i]) != NULL) {
            return 1; // Threat detected
        }
    }
    return 0; // Clean
}