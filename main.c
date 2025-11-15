#include "hashtable.h" 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
typedef struct Email {
    char sender[50];
    char recipient[50];
    char subject[100];
    char bodyFile[100];      
    char attachments[3][50]; 
    int numAttachments;
} Email;

Email emailInbox[MAX_QUEUE];
int emailCount = 0; 

int processEmail(
    Email* email, 
    Hushtable* senderHT, 
    Hushtable* keywordHT, 
    FileStack* fs
) {
    int isSpam = 0;
    
    printf("\n--- Analyzing Email from %s ---\n", email->sender);

    // --- 1. Sender Reputation Check (Hash Table) ---
    updatefreq(senderHT, email->sender);

    if (isblacked(senderHT, email->sender)) {
        printf("[ SENDER CHECK] Sender %s is BLACKLISTED. Score: +100\n", email->sender);
        return 1; 
    }
    if (iswhite(senderHT, email->sender)) {
        printf("[ SENDER CHECK] Sender %s is WHITELISTED. Email is SAFE.\n", email->sender);
        return 0;
    }

    -
    if (checkEmailForSpam(keywordHT, email->bodyFile)) {
        printf("[ KEYWORD CHECK] Content exceeded spam score threshold (>= 10). Score: +10\n");
        isSpam = 1;
    } else {
        printf("[KEYWORD CHECK] Content is below spam score threshold.\n");
    }

    
    int hasThreat = 0;
    for (int i = 0; i < email->numAttachments; i++) {
        if (strlen(email->attachments[i]) > 0) {
            addFile(fs, email->attachments[i]);
        }
    }

    char* filename;
    while (!checkEmpty(fs)) {
        filename = removeFile(fs);
        if (filename != NULL) {
            if (detectThreat(filename)) {
                printf("[ ATTACHMENT CHECK] Detected THREAT in file: %s\n", filename);
                hasThreat = 1;
            } else {
                printf("[ ATTACHMENT CHECK] File %s is clean.\n", filename);
            }
            free(filename);
        }
    }

    if (hasThreat) {
        printf("[ FINAL RATING] EMAIL IS A HIGH THREAT (Attachment).\n");
        return 1;
    }
    
    if (isSpam) {
        printf("[ FINAL RATING] EMAIL IS LIKELY SPAM.\n");
        return 1;
    }

    printf("[ FINAL RATING] EMAIL IS CLEAN.\n");
    return 0;
}

void printMenu() {
    printf("\n--- Spam Filter Interactive Menu ---\n");
    printf("1. Add a new email to the queue\n");
    printf("2. Process all emails in the queue\n");
    printf("3. Manually add sender to BLACKLIST\n");
    printf("4. Manually add sender to WHITELIST\n");
    printf("5. Display sender database (from memory)\n");
    printf("6. Save database and Exit\n");
    printf("--------------------------------------\n");
    printf("Enter your choice: ");
}

int main(int argc, char *argv[]) {
    // --- 1. Initialization (Runs for all modes) ---
    Hushtable senderHT;   
    Hushtable keywordHT;  
    FileStack* fileStack = initStack();
    graph* emailGraph = createg();
    Queue* emailQueue = create_queue(); 

    createtable(&senderHT);
    createtable(&keywordHT);
    
    loadSpamKeywords(&keywordHT, "keywords.txt");
    loadSenderDatabase(&senderHT, "sender_database.csv");
    addwhite(&senderHT, "boss@company.com");
    addwhite(&senderHT, "trusted_contact@personal.org");
    
    edge(emailGraph, "hacker@malware.net", "trusted_contact@personal.org"); 
    edge(emailGraph, "trusted_contact@personal.org", "employee@company.com");
    edge(emailGraph, "boss@company.com", "employee@company.com"); 
    
    if (argc == 3 && strcmp(argv[1], "-blacklist") == 0) {
        addblack(&senderHT, argv[2]);
        saveSenderDatabase(&senderHT, "sender_database.csv");
        printf("SUCCESS: %s added to blacklist.", argv[2]);
        return 0;
    }
    
    if (argc == 3 && strcmp(argv[1], "-whitelist") == 0) {
        addwhite(&senderHT, argv[2]);
        saveSenderDatabase(&senderHT, "sender_database.csv");
        printf("SUCCESS: %s added to whitelist.", argv[2]);
        return 0;
    }

    if (argc >= 3) {
        
        Email tempEmail;
        strncpy(tempEmail.sender, argv[1], 50);
        strncpy(tempEmail.bodyFile, argv[2], 100);

        tempEmail.numAttachments = argc - 3;
        if (tempEmail.numAttachments > 3) tempEmail.numAttachments = 3;

        for (int i = 0; i < tempEmail.numAttachments; i++) {
            strncpy(tempEmail.attachments[i], argv[i + 3], 50);
        }
        
        int result = processEmail(&tempEmail, &senderHT, &keywordHT, fileStack);

        if (result == 1) {
            printf("\nFINAL_RESULT:BLOCKED\n");
        } else {
            printf("\nFINAL_RESULT:CLEAN\n");
        }
        
        saveSenderDatabase(&senderHT, "sender_database.csv");
        return 0; 
    }
    
    printf("--- SPAM FILTER INITIALIZATION ---\n");
    printf("[DB LOAD] Loaded spam keyword database from 'keywords.txt'.\n");
    printf("[DB LOAD] Successfully loaded sender database.\n");
    printf("[CONFIG] Applied manual Blacklist/Whitelist rules.\n");
    printf("[GRAPH] Built email relationship graph.\n");
    
    int choice = 0;
    while (1) {
        printMenu();
        if (scanf("%d", &choice) != 1) {
            while (fgetc(stdin) != '\n');
            choice = 0; 
        }

        switch (choice) {
            case 1: { // Add a new email to the queue
                if (emailCount >= MAX_QUEUE) {
                    printf("Email inbox is full! Please process the queue first.\n");
                    break;
                }

                Email tempEmail;
                printf("Enter sender email: ");
                scanf("%s", tempEmail.sender);

                printf("Enter recipient email: ");
                scanf("%s", tempEmail.recipient);

                while (fgetc(stdin) != '\n'); 

                printf("Enter subject: ");
                scanf("%[^\n]s", tempEmail.subject);

                printf("Enter body filename (e.g., test_spam_body.txt): ");
                scanf("%s", tempEmail.bodyFile);

                printf("How many attachments (0-3)? ");
                scanf("%d", &tempEmail.numAttachments);

                if (tempEmail.numAttachments > 3) tempEmail.numAttachments = 3;
                for (int i = 0; i < tempEmail.numAttachments; i++) {
                    printf("Enter attachment %d filename: ", i + 1);
                    scanf("%s", tempEmail.attachments[i]);
                }
                for (int i = tempEmail.numAttachments; i < 3; i++) {
                    strcpy(tempEmail.attachments[i], ""); 
                }

                emailInbox[emailCount] = tempEmail;
                char indexStr[10];
                sprintf(indexStr, "%d", emailCount);
                enqueue(emailQueue, indexStr);

                emailCount++; 
                printf("Email from %s queued. Queue size: %d\n", tempEmail.sender, queue_size(emailQueue));
                break;
            }
            
            case 2: { // Process all emails in the queue
                printf("\n--- Starting Email Processing from Queue ---\n");
                if (is_empty(emailQueue)) {
                    printf("[QUEUE] Queue is empty. Nothing to process.\n");
                    break;
                }
                while (!is_empty(emailQueue)) {
                    char* emailIndexStr = dequeue(emailQueue);
                    if (emailIndexStr == NULL) continue;

                    int i = atoi(emailIndexStr);
                    
                    int result = processEmail(&emailInbox[i], &senderHT, &keywordHT, fileStack);
                    printf("--- Result for Email (Index %d): %s ---\n", i, result ? "BLOCKED/QUARANTINED" : "DELIVERED");

                    FILE* logFile = fopen("filter_log.txt", "a");
                    if (logFile) {
                        const char* status = result ? "BLOCKED" : "DELIVERED";
                        fprintf(logFile, "Email from: %s | To: %s | Subject: \"%s\" | Final Status: %s\n",
                                emailInbox[i].sender, emailInbox[i].recipient, emailInbox[i].subject, status);
                        fclose(logFile);
                    }
                    free(emailIndexStr); 
                }
                printf("[QUEUE] Email processing complete. Queue is now empty.\n");
                emailCount = 0; 
                break;
            }

            case 3: { // Manually add sender to BLACKLIST
                char email[100];
                printf("Enter email to BLACKLIST: ");
                scanf("%s", email);
                addblack(&senderHT, email);
                printf("Sender %s added to blacklist.\n", email);
                break;
            }
            
            case 4: { // Manually add sender to WHITELIST
                char email[100];
                printf("Enter email to WHITELIST: ");
                scanf("%s", email);
                addwhite(&senderHT, email);
                printf("Sender %s added to whitelist.\n", email);
                break;
            }

            case 5: { // Display sender database
                printf("\n--- Current Sender Database (in memory) ---\n");
                display(&senderHT); 
                break;
            }

            case 6: { // Save database and Exit
                printf("\n--- Filter Shutdown ---\n");
                saveSenderDatabase(&senderHT, "sender_database.csv");
                
                // Cleanup
                free(fileStack);
                free(emailGraph);
                while (!is_empty(emailQueue)) {
                    free(dequeue(emailQueue));
                }
                free(emailQueue);
                
                printf("Goodbye!\n");
                return 0; // Exit the program
            }

            default:
                printf("Invalid choice. Please enter a number from 1 to 6.\n");
                break;
        }
    }

    return 0;
}