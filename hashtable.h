#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h> // For true/false

#define Tablesize 500

// Node for the hash table chain
typedef struct hash {
    char sender[50];
    int freq;
    int status; // 0 = normal, 1 = blacklisted, 2 = whitelisted
    struct hash* next;
} hash;

// The hash table structure
typedef struct Hushtable {
    hash* table[Tablesize];
} Hushtable;

// Function prototypes for hashmap.c
unsigned int hush_function(const char* str);
void createtable(Hushtable* ht);
hash* createnode(const char* sender);
void initsneder(Hushtable* ht, const char* sender);
void updatefreq(Hushtable* ht, const char* sender);
void addblack(Hushtable* ht, const char* sender);
void addwhite(Hushtable* ht, const char* sender);
int isblacked(Hushtable* ht, const char* sender);
int iswhite(Hushtable* ht, const char* sender);
void display(Hushtable* ht);
void saveSenderDatabase(Hushtable* ht, const char* filename); 

#define LIMIT_STACK 100

typedef struct FileStack {
    char* files[LIMIT_STACK];
    int head;
} FileStack;

// Function prototypes for stack.c
FileStack* initStack();
bool checkEmpty(FileStack* s);
bool checkFull(FileStack* s);
void addFile(FileStack* s, const char* filename);
char* removeFile(FileStack* s);
int detectThreat(const char* filename);

#define MAX_QUEUE 100 

typedef struct Queue {
    char* items[MAX_QUEUE];   
    int front;          
    int rear;           
} Queue;

// Function prototypes for queue.c
Queue* create_queue();                       
bool is_empty(Queue* queue);                  
bool is_full(Queue* queue);         
void enqueue(Queue* queue, const char* recipient); 
char* dequeue(Queue* queue);                 
int queue_size(Queue* queue);  

#define MAX_NODES 100

// Node for the graph adjacency list
typedef struct graphnode {
    char sender[50]; // Represents the recipient in the edge
    struct graphnode* next;
} graphnode;

// The graph structure
typedef struct graph {
    graphnode* arr[MAX_NODES]; // <-- THIS IS THE CORRECTED LINE
    int size;
} graph;

// Function prototypes for graph.c
graph* createg();
void edge(graph* g, const char* sender, const char* recipient);
void displayg(graph* g);
void bfscan(graph* g, Hushtable* ht, const char* start);


// Function prototypes for filehandling.c
void loadSpamKeywords(Hushtable* ht, const char* filename);
int checkEmailForSpam(Hushtable* ht, const char* emailFile);
// --- NEW FUNCTION PROTOTYPE ---
void loadSenderDatabase(Hushtable* ht, const char* filename);


#endif // HASHTABLE_H