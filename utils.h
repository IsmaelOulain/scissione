#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h> 
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <time.h>
#include "semaforo_binario.h"
#include "shared_array.h"

//Costanti per attivatore
#define STEP_ATTIVATORE 2
#define RANDOM_ATOM_SET 3 //N atomi da scegliere per inviare comando

//Costanti per master e atomo
#define N_ATOMI_INIT 5
#define N_ATOM_MAX 10 // Massimo valore per il numero atomico
#define MIN_NUM_ATOM 2 //Costante che identifica il numero atomico minimo per scindersi
#define SIM_DURATION 30 //Durata della costante
#define ENERGY_DEMAND 2 //Quantita' di energia da assorbire
#define ENERGY_EXPLODE_THRESHOLD 50 //Valore massimo di energia per evitare EXPLODE
#define ENERGY_INIT 30 //Valore di energia iniziale

//Costanti per alimentatore
#define N_NUOVI_ATOMI 1       // Numero di nuovi atomi da creare ad ogni step
#define STEP_ALIMENTAZIONE 10 // Intervallo di tempo tra le creazioni, in secondi
