#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashtable.h"
//  Hash function (djb2 algorithm)
unsigned int hush_function(const char* str) {
    unsigned long hash = 5381;  
    int c;
    while ((c = *str++)) {       
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    return hash % Tablesize;    
}

void createtable(Hushtable* ht) {
    for (int i = 0; i < Tablesize; i++) {
        ht->table[i] = NULL;     // set all buckets to NULL
    }
}

hash* createnode(const char* sender) {
    hash* newNode = (hash*)malloc(sizeof(hash));
    if (!newNode) {
        fprintf(stderr, "Memory allocation failed!\n");
        exit(1);
    }
    strncpy(newNode->sender, sender, sizeof(newNode->sender) - 1);
    newNode->sender[sizeof(newNode->sender) - 1] = '\0';
    newNode->freq = 1;
    newNode->status = 0;          
    newNode->next = NULL;
    return newNode;
}

void initsneder(Hushtable* ht, const char* sender) {
    int idx = hush_function(sender);    
    hash* temp = ht->table[idx];

    while (temp != NULL) {
        if (strcmp(temp->sender, sender) == 0) {
            temp->freq++;               
            return;
        }
        temp = temp->next;
    }

    hash* newNode = createnode(sender);  
    newNode->next = ht->table[idx];
    ht->table[idx] = newNode;            
}
void updatefreq(Hushtable* ht, const char* sender) {
    int idx = hush_function(sender);
    hash* temp = ht->table[idx];

    while (temp != NULL) {
        if (strcmp(temp->sender, sender) == 0) {
            temp->freq++;
            return;
        }
        temp = temp->next;
    }

    initsneder(ht, sender);  // if not found, insert new
}

void addblack(Hushtable* ht, const char* sender) {
    int idx = hush_function(sender);
    hash* temp = ht->table[idx];

    while (temp != NULL) {
        if (strcmp(temp->sender, sender) == 0) {
            temp->status = 1;       // blacklisted
            return;
        }
        temp = temp->next;
    }

    initsneder(ht, sender);       // insert if not found
    ht->table[idx]->status = 1;
}

void addwhite(Hushtable* ht, const char* sender) {
    int idx = hush_function(sender);
    hash* temp = ht->table[idx];

    while (temp != NULL) {
        if (strcmp(temp->sender, sender) == 0) {
            temp->status = 2;       // whitelisted
            return;
        }
        temp = temp->next;
    }

    initsneder(ht, sender);
    ht->table[idx]->status = 2;
}

int isblacked(Hushtable* ht, const char* sender) {
    int idx = hush_function(sender);
    hash* temp = ht->table[idx];

    while (temp != NULL) {
        if (strcmp(temp->sender, sender) == 0) {
            return temp->status == 1;
        }
        temp = temp->next;
    }
    return 0;
}

int iswhite(Hushtable* ht, const char* sender) {
    int idx = hush_function(sender);
    hash* temp = ht->table[idx];

    while (temp != NULL) {
        if (strcmp(temp->sender, sender) == 0) {
            return temp->status == 2;
        }
        temp = temp->next;
    }
    return 0;
}
void display(Hushtable* ht) {
    for (int i = 0; i < Tablesize; i++) {
        hash* temp = ht->table[i];
        if (temp != NULL) {
            printf("Index %d -> ", i);
            while (temp != NULL) {
                printf("[Sender: %s | Freq: %d | Status: %d] -> ", 
                       temp->sender, temp->freq, temp->status);
                temp = temp->next;
            }
            printf("NULL\n");
        }
    }
}
void saveSenderDatabase(Hushtable* ht, const char* filename) {
    FILE* fp = fopen(filename, "w"); // Open in "write" mode (creates new/overwrites)
    if (fp == NULL) {
        printf("ERROR: Could not open file %s for writing.\n", filename);
        return;
    }


    fprintf(fp, "SenderEmail,Frequency,Status\n");

    for (int i = 0; i < Tablesize; i++) {
        hash* temp = ht->table[i];
        
        while (temp != NULL) {
            char statusStr[20];
            
            // Convert status number to a readable string
            if (temp->status == 1) {
                strcpy(statusStr, "Blacklisted");
            } else if (temp->status == 2) {
                strcpy(statusStr, "Whitelisted");
            } else {
                strcpy(statusStr, "Normal");
            }

            // Write the data as a CSV (Comma-Separated Values) row
            fprintf(fp, "%s,%d,%s\n", temp->sender, temp->freq, statusStr);
            
            temp = temp->next;
        }
    }

    fclose(fp); // Close the file
    printf("[DB SAVE] Sender database saved to %s\n", filename);
}