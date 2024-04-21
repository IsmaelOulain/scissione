// pidlist.h
#ifndef PIDLIST_H
#define PIDLIST_H

#include <sys/types.h>

typedef struct Nodo
{
    pid_t pid;
    struct Nodo *next;
} Nodo;

void push(Nodo **head, pid_t pid);
pid_t pop(Nodo **head);
void stampaLista(Nodo *head);
void liberaLista(Nodo **head);

#endif
