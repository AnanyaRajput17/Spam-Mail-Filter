#include "hashtable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// Load spam keywords from file into the keyword hash table
void loadSpamKeywords(Hushtable* ht, const char* filename) {
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        perror("Failed to open spam keyword file");
        return;
    }

    char keyword[100];
    // Read one "word" (keyword) at a time
    while (fscanf(fp, "%s", keyword) != EOF) {
        addblack(ht, keyword); 
    }

    fclose(fp);
}

// Check a given email body file for spam keywords
int checkEmailForSpam(Hushtable* ht, const char* emailFile) {
    FILE* fp = fopen(emailFile, "r");
    if (!fp) {
        perror("Failed to open email file");
        return 0; // Return 0 (not spam) if file can't be opened
    }

    char word[100];
    int spamScore = 0;

    // Read one word at a time from the email body
    while (fscanf(fp, "%s", word) != EOF) {
        if (isblacked(ht, word)) {
            int idx = hush_function(word);
            hash* temp = ht->table[idx];
            while(temp) {
                if (strcmp(temp->sender, word) == 0) {
                    spamScore += temp->freq; 
                    break;
                }
                temp = temp->next;
            }
        }
    }

    fclose(fp);
    
    return spamScore >= 10;  
}

void loadSenderDatabase(Hushtable* ht, const char* filename) {
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        printf("[DB LOAD] No existing sender database found. Starting fresh.\n");
        return;
    }

    char line[256];
    fgets(line, sizeof(line), fp);

    while (fgets(line, sizeof(line), fp)) {
        char* email = strtok(line, ",");
        char* freqStr = strtok(NULL, ",");
        char* status = strtok(NULL, "\n"); 

        if (email && freqStr && status) {
            int freq = atoi(freqStr); 

            if (strcmp(status, "Blacklisted") == 0) {
                addblack(ht, email);
            } else if (strcmp(status, "Whitelisted") == 0) {
                addwhite(ht, email);
            } else {
                initsneder(ht, email);
            }

            int idx = hush_function(email);
            hash* temp = ht->table[idx];
            while (temp) {
                if (strcmp(temp->sender, email) == 0) {
                    temp->freq = freq; 
                    break;
                }
                temp = temp->next;
            }
        }
    }

    fclose(fp);
    printf("[DB LOAD] Successfully loaded sender database from %s\n", filename);
}