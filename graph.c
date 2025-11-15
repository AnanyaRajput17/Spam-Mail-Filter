#include "hashtable.h"  
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//graph creation
graph* createg() {
    graph* g=(graph*)malloc(sizeof(graph));
    g->size=MAX_NODES;
    for(int i=0; i<MAX_NODES; i++) {
        g->arr[i]=NULL;
    }
    return g;
}

//add edge (sender->recipient)
void edge(graph* g, const char* sender, const char* recipient) {
    int idx=abs((int)sender[0])%MAX_NODES;   //hash based on sender
    graphnode* new=(graphnode*)malloc(sizeof(graphnode));
    strncpy(new->sender, recipient, sizeof(new->sender) - 1); // Store recipient
    new->sender[sizeof(new->sender)-1]='\0';
    new->next=g->arr[idx];
    g->arr[idx]=new;
}

//display connections of graph
void displayg(graph* g) {
    printf("\n EMAIL RELATION GRAPH:\n");
    for(int i=0; i<MAX_NODES; i++) {
        if(g->arr[i]) {
            printf("NODE[%d]->", i); // This index is hashed from a sender
            graphnode* temp=g->arr[i];
            while(temp) {
                printf("%s ->", temp->sender); // This is a recipient
                temp=temp->next;
            }
            printf("\n");
        }
    }
}

//BFS scan to detect any dangerous relations
void bfscan(graph* g, Hushtable* ht, const char* start) {
    int visit[MAX_NODES]={0};
    char q[MAX_NODES][50];
    int front=0, rear=0;

    strncpy(q[rear++], start, 50);
    visit[abs((int)start[0])%MAX_NODES]=1;
    printf("\n Scanning chain starting from: %s\n", start);

    while(front<rear) {
        char curr[50];
        strncpy(curr, q[front++], 50);
        int iidx=abs((int)curr[0])%MAX_NODES;
        
        graphnode* temp=g->arr[iidx];
        while(temp) {
            //check sender-receiver relation
            printf("Checking link: %s -> %s\n", curr, temp->sender);
            
            //integrate hashtable checks
            if(isblacked(ht, temp->sender)) { // Checking the recipient
                printf("ALERT: %s is blacklisted!\n", temp->sender);
            } else if(iswhite(ht, temp->sender)) {
                printf("Safe: %s is whitelisted.\n", temp->sender);
            } else {
                printf("Unknown: %s not in whitelist/blacklist.\n", temp->sender);
            }
            
            //continue bfs if not visited
            int nxtidx=abs((int)temp->sender[0])%MAX_NODES;
            if(!visit[nxtidx]) {
                strncpy(q[rear++], temp->sender, 50); 
                visit[nxtidx]=1;
            }
            temp=temp->next;
        }
    }
}
