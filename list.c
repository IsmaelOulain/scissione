#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

typedef struct Nodo
{
    pid_t pid;
    struct Nodo *next;
} Nodo;

// Aggiungi un nuovo PID alla lista
void push(Nodo **head, pid_t pid)
{
    Nodo *nuovoNodo = malloc(sizeof(Nodo));
    if (nuovoNodo == NULL)
    {
        perror("Errore nell'allocazione della memoria");
        exit(EXIT_FAILURE);
    }
    nuovoNodo->pid = pid;
    nuovoNodo->next = *head;
    *head = nuovoNodo;
}

// Rimuovi un PID specifico dalla lista
int pop(Nodo **head, pid_t pidDaRimuovere)
{
    Nodo *temp = *head, *prev = NULL;
    // Se il nodo da rimuovere è la testa della lista
    if (temp != NULL && temp->pid == pidDaRimuovere)
    {
        *head = temp->next; // Cambia la testa
        free(temp);         // Libera la vecchia testa
        return 1;           // Successo
    }

    // Cerca il pid da rimuovere, tenendo traccia del nodo precedente
    while (temp != NULL && temp->pid != pidDaRimuovere)
    {
        prev = temp;
        temp = temp->next;
    }

    // Se il pid non era presente nella lista
    if (temp == NULL)
        return 0; // Fallimento

    // Stacca il nodo dalla lista
    prev->next = temp->next;

    // Libera la memoria
    free(temp);
    return 1; // Successo
}

// Stampa tutti i PID nella lista
void stampaLista(Nodo *head)
{
    Nodo *current = head;
    while (current != NULL)
    {
        printf("%d ", current->pid);
        current = current->next;
    }
    printf("\n");
}

// Libera tutta la memoria allocata per la lista
void liberaLista(Nodo **head)
{
    Nodo *current = *head;
    Nodo *next = NULL;

    while (current != NULL)
    {
        next = current->next;
        free(current);
        current = next;
    }
    *head = NULL;
}