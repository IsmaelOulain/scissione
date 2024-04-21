// messaggio_atomo.h

#ifndef MESSAGGIO_ATOMO_H
#define MESSAGGIO_ATOMO_H

#include <sys/types.h>

// Definizione dell'enum per i tipi di messaggi
typedef enum
{
    CREAZIONE_ATOMO = 1,
    SCISSONE,
    TERMINAZIONE_ATTIVATORE,
    CREAZIONE_ATTIVATORE,
    CREAZIONE_ALIMENTATORE,
    SCORIA
} TipoMessaggio;

typedef struct
{
    TipoMessaggio mtype; // Tipo di messaggio (1 per creazione/scissione, 2 per terminazione)
    struct
    {
        pid_t pid;
        pid_t ppid;
        int numAtomico;
    } info;
} MessaggioAtomo;

#endif // MESSAGGIO_ATOMO_H
