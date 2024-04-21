#include "shared_array.h"
#include <stdio.h>
#include <stdlib.h>
void push(SharedArray *sharedArray, int value)
{
    if (sharedArray->count < MAX_SIZE)
    {
        sharedArray->data[sharedArray->count++] = value;
    }
    else
    {
        printf("L'array condiviso è pieno.\n");
    }
}

int pop(SharedArray *sharedArray, int value)
{
    int index = -1;
    // Cerca l'indice del valore da rimuovere
    for (int i = 0; i < sharedArray->count; i++)
    {
        if (sharedArray->data[i] == value)
        {
            index = i;
            break;
        }
    }

    if (index != -1) // Se il valore è stato trovato
    {
        // Sposta tutti i valori successivi una posizione indietro
        for (int i = index; i < sharedArray->count - 1; i++)
        {
            sharedArray->data[i] = sharedArray->data[i + 1];
        }
        sharedArray->count--; // Riduce il conteggio dei valori nell'array
        return value;         // Ritorna il valore rimosso
    }
    else
    {
        printf("Valore %d non trovato nell'array.\n", value);
        return -1; // Valore di errore, valore non trovato
    }
}

int get(SharedArray *sharedArray)
{
    if (sharedArray->count > 0)
    {
        // Restituisce l'ultimo elemento senza modificare il contatore
        return sharedArray->data[sharedArray->count - 1];
    }
    else
    {
        printf("L'array condiviso è vuoto.\n");
        return -1; // Valore di errore
    }
}

void printSharedArray(const SharedArray *sharedArray)
{
    printf("Contenuto dell'array condiviso: ");
    for (int i = 0; i < sharedArray->count; i++)
    {
        printf("%d ", sharedArray->data[i]);
    }
    printf("\n");
}

int *selectRandomValues(const SharedArray *sharedArray, int n, int *selectedSize)
{
    if (sharedArray->count == 0)
    {
        //printf("Non ci sono atomi\n");
        *selectedSize = 0;
        return NULL;
    }

    // Se N è maggiore del numero di elementi disponibili, seleziona tutti gli elementi
    if (n > sharedArray->count)
    {
        n = sharedArray->count;
    }

    *selectedSize = n; // Numero di elementi selezionati
    int *values = malloc(n * sizeof(int));

    // Creare un array di indici per evitare selezioni ripetute
    int *indices = malloc(sharedArray->count * sizeof(int));
    if (!indices)
    {
        printf("Errore nell'allocazione della memoria per gli indici.\n");
        *selectedSize = 0;
        return NULL;
    }
    for (int i = 0; i < sharedArray->count; i++)
    {
        indices[i] = i;
    }

    // Seleziona N valori casuali
    for (int i = 0; i < n; i++)
    {
        int randomIndex = rand() % (sharedArray->count - i); // Seleziona un indice casuale tra quelli rimasti
        values[i] = sharedArray->data[indices[randomIndex]]; // Assegna il valore selezionato
        // Rimuove l'indice selezionato scambiandolo con l'ultimo elemento non selezionato
        indices[randomIndex] = indices[sharedArray->count - i - 1];
    }
    free(indices);
    return values;
}

int *getAllValues(const SharedArray *sharedArray, int *selectedSize)
{
    if (sharedArray->count == 0)
    {
        printf("Non ci sono atomi.\n");
        *selectedSize = 0;
        return NULL;
    }

    *selectedSize = sharedArray->count; // Numero di elementi presenti nell'array
    int *values = malloc(*selectedSize * sizeof(int));

    if (values == NULL)
    {
        printf("Errore nell'allocazione della memoria.\n");
        *selectedSize = 0;
        return NULL;
    }

    // Copia tutti i valori presenti nell'array condiviso
    for (int i = 0; i < *selectedSize; i++)
    {
        values[i] = sharedArray->data[i];
    }

    return values;
}


int isEmpty(const SharedArray *sharedArray)
{
    return sharedArray->count == 0;
}
