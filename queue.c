#include "hashtable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
Queue* create_queue() {
    Queue* queue = (Queue*)malloc(sizeof(Queue));
    queue->front = -1;
    queue->rear = -1;
    return queue;
}

bool is_empty(Queue* queue) {
    if(queue->front == -1)
          return true;
    return false; // Added explicit false return
}

bool is_full(Queue* queue) {
    if((queue->rear + 1) % MAX_QUEUE == queue->front) // Fixed parenthesis
          return true;
    return false; // Added explicit false return
}

void enqueue(Queue* queue, const char* recipient) {
    if (is_full(queue)) {
        printf("Queue overflow! Cannot enqueue %s\n", recipient);
        return;
    }
    if (is_empty(queue)) {
        queue->front = 0;
    }
    queue->rear = (queue->rear + 1) % MAX_QUEUE;
    queue->items[queue->rear] = strdup(recipient);
}

char* dequeue(Queue* queue) {
    if (is_empty(queue)) {
        printf("Queue underflow!\n");
        return NULL;
    }
    char* front_item = queue->items[queue->front];
    if (queue->front == queue->rear) {
        // Queue is now empty
        queue->front = queue->rear = -1;
    } else {
        queue->front = (queue->front + 1) % MAX_QUEUE;
    }
    return front_item;
}


int queue_size(Queue* queue) {
    if (is_empty(queue)){
           return 0;
    }
    if (queue->rear >= queue->front){
        return queue->rear - queue->front + 1;
    }
    return MAX_QUEUE - queue->front + queue->rear + 1;
}