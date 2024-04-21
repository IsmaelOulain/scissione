#ifndef SHARED_ARRAY_H
#define SHARED_ARRAY_H

#include <sys/types.h>

#define MAX_SIZE 100 // Dimensione massima dell'array condiviso

// Struttura per gestire l'array condiviso
typedef struct
{
    int data[MAX_SIZE];
    int count; // Numero attuale di elementi nell'array
} SharedArray;

// Funzioni per manipolare l'array
void push(SharedArray *sharedArray, int value);
int pop(SharedArray *sharedArray, int value);
int get(SharedArray *sharedArray);
void printSharedArray(const SharedArray *sharedArray);
int *selectRandomValues(const SharedArray *sharedArray, int n, int *selectedSize);
int *getAllValues(const SharedArray *sharedArray, int *selectedSize);
int isEmpty(const SharedArray *sharedArray);

#endif // SHARED_ARRAY_H
