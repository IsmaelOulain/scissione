#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include "messaggio_atomo.h"
#include "semaforo_binario.h"
#include "statistiche.h"
#define N_NUOVI_ATOMI 1       // Numero di nuovi atomi da creare ad ogni step
#define STEP_ALIMENTAZIONE 10 // Intervallo di tempo tra le creazioni, in secondi
#define N_ATOM_MAX 3

pid_t pid;
int shmid;
int shmidStats;
int semIdStats;
int pidMaster;
StatisticheSimulazione *statisticheSimulazione;
void alarmHandler(int sig);
void createAtomo();
void alimentazione();
void inviaMessaggio(TipoMessaggio tipo);
void attendi_start();
void start(int sig);
void waiting();
void saveStats();
int main()
{
    semIdStats = atoi(getenv("SEMAFORO_STATISTICHE"));
    shmidStats = atoi(getenv("STATS_ID"));
    pidMaster = atoi(getenv("PID_MASTER"));
    inviaMessaggio(CREAZIONE_ALIMENTATORE);
    attendi_start();
    srand(time(NULL));


    return 0;
}


void stepAlimentatore(){

    //printf("Avvio simulazione attivatore\n");
    // Imposta l'handler per SIGALRM
    struct sigaction sa;
    sa.sa_handler = alarmHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGALRM, &sa, NULL);

    alarm(STEP_ALIMENTAZIONE); // Inizia il timer per l'alimentazione

    // Loop principale
    while (1)
    {
        pause(); // Attendi il prossimo segnale (interruzione bloccante)
    }
}

void attendi_start()
{
    struct sigaction sa;
    sa.sa_handler = stepAlimentatore; // Imposta la funzione di gestione del segnale
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGUSR1, &sa, NULL) == -1)
    {
        perror("Errore in sigaction");
        exit(EXIT_FAILURE);
    }
    pause();
}

void start(int sig)
{

   //printf("Avvio simulazione alimentatore\n");
    stepAlimentatore();
}

// Simula la creazione di un nuovo atomo
void createAtomo()
{
    int flag_master=0;
    for (int i = 0; i < N_NUOVI_ATOMI; i++)
    {
        // Genero un numero atomico casuale tra 1 e N_ATOM_MAX
        int atomicNumber = (rand() % N_ATOM_MAX) + 1;
        
        char atomicNumberStr[10];
        sprintf(atomicNumberStr, "%d", atomicNumber);
        switch (pid = fork())
        {
        case -1:
            printf("MELTDOWN");
            kill(pidMaster,SIGUSR2);
            break;
        case 0:
            //printf("creato %d\n",getpid());
            /* Perform actions specific to child */
            execlp("./atomo", "atomo", atomicNumberStr,"0", NULL);
            break;

        default:

            break;
        }
    }

    waiting();
    //printf("\n");
}



// Handler per il segnale SIGALRM che chiama alimentazione()
void alarmHandler(int sig)
{
    //printf("STEP ALIMENTATORE: creo nuovi atomi\n");
    P(semIdStats);
    saveStats();
    V(semIdStats);
    createAtomo();             // Crea nuovi atomi
    //printf("\n");
    alarm(STEP_ALIMENTAZIONE); // Resetta l'alarm per il prossimo step
}

void inviaMessaggio(TipoMessaggio tipo)
{
    char *msgIdStr = getenv("MSGATM_ID");
    int msg_id = atoi(msgIdStr);
    MessaggioAtomo msg;
    msg.mtype = tipo;
    msg.info.pid = getpid();

    msgsnd(msg_id, &msg, sizeof(msg.info), 0);
    // exit(EXIT_SUCCESS);
}

void waiting()
{
    int status;
    while ((pid = waitpid(-1, &status, WNOHANG)) != -1)
    {
        // printf("PARENT: PID=%d. Got info of child with PID=%d, status=0x%04X\n", getpid(), pid, status);
        //  printf("PARENT: PID=%d. Got info of child with PID=%d, status=%d\n", getpid(), child_pid, WEXITSTATUS(status));
    }
    if (errno == ECHILD)
    {
        //printf("In PID=%6d, no more child processes\n", getpid());
        // printSharedArray(sharedArray);
        //  Pulizia: rimuovi il semaforo quando non è più necessario
    }
    else
    {
        fprintf(stderr, "Error #%d: %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }
}

void saveStats()
{

    if (shmidStats == 0)
    {
        fprintf(stderr, "SHMID non impostato.\n");
        exit(1);
    }

    statisticheSimulazione = (StatisticheSimulazione *)shmat(shmidStats, NULL, 0);
    if (statisticheSimulazione == (void *)-1)
    {
        perror("shmat");
        exit(1);
    }
    // printf("atomo tot %d\n", statisticheSimulazione->scissioniTotali);
    statisticheSimulazione->alimentazioniTotali++;
}