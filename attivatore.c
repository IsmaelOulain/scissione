#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <signal.h>
#include "shared_array.h"
#include "attivatore.h"
#include "semaforo_binario.h"
#include "statistiche.h"

#define STEP_ATTIVATORE 2
#define RANDOM_ATOM_SET 3
int semId;
int semIdStats;
int shmidStats;
int countStepAttivatore=0;
StatisticheSimulazione *statisticheSimulazione;
int main()
{
    inviaMessaggio(CREAZIONE_ATTIVATORE);
    semId = atoi(getenv("SEMAFORO_ATOMI"));
    semIdStats = atoi(getenv("SEMAFORO_STATISTICHE"));
    shmidStats = atoi(getenv("STATS_ID"));
    attendi_start();
    //printf("Sono attivatore shmid %s\n", getenv("SHMPIDATOMI"));
    
    //printf("Sono attivatore \n");
    // int pid_atomo = get(sharedArray);
    // kill(pid_atomo, SIGUSR2);

    //sezione critica
    
    //printf("%d\\n",semId);
    //attendi_start();
    //printf("\n");

    //exit(EXIT_SUCCESS);
}

void attendi_start()
{

    //printf("\n");
    struct sigaction sa;
    sa.sa_handler = stepAttivatore; // Imposta la funzione di gestione del segnale
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    // Configura il gestore per SIGUSR2
    if (sigaction(SIGUSR1, &sa, NULL) == -1)
    {
        perror("Errore in sigaction");
        exit(EXIT_FAILURE);
    }
    pause();
}



void start(int sig)
{

    //printf("Avvio simulazione attivatore\n");
    stepAttivatore();
}

void stepAttivatore()
{
    //printf("Avvio simulazione attivatore\n");
    //attendi_stop();
    signal(SIGALRM, comandoScissione);

    // Imposta il primo alarm
    alarm(STEP_ATTIVATORE);

    // Ciclo infinito per mantenere in esecuzione il processo attivatore
    while (1)
    {
        pause(); // Mette in pausa il processo in attesa di segnali
    }
}

int * arrayPid(int *selectedSize)
{
    int shmid = atoi(getenv("SHMPIDATOMI"));
    if (shmid == 0)
    {
        fprintf(stderr, "SHMID non impostato.\n");
        *selectedSize = 0; // Imposta la dimensione selezionata a 0 in caso di errore
        return NULL;
    }

    SharedArray *sharedArray = (SharedArray *)shmat(shmid, NULL, 0);
    if (sharedArray == (void *)-1)
    {
        perror("shmat");
        *selectedSize = 0; // Imposta la dimensione selezionata a 0 in caso di errore
        return NULL;
    }

    int *selectedValues = selectRandomValues(sharedArray, RANDOM_ATOM_SET, selectedSize);
    // Non è necessario stampare o liberare selectedValues qui

    return selectedValues; // Restituisce il puntatore agli elementi selezionati
}




void comandoScissione(int signum)
{



    //printf("totale attivazioni: %d\n",countStepAttivatore++);
    P(semId);

    int selectedSize;
    int *selectedValues = arrayPid(&selectedSize);

    if (selectedValues != NULL)
    {

        P(semIdStats);
        saveStats();
        V(semIdStats);
        //stats();
        for (int i = 0; i < selectedSize; i++)
        {
            int pid_atomo = selectedValues[i];
            //printf("%d\n",pid_atomo);
            kill(pid_atomo, SIGUSR2);
        }
        //printf("\n");
        free(selectedValues); // Libera la memoria allocata per i valori selezionati
    }
    else
    {
        //printf("Non è stato possibile selezionare valori.\n");
    }

    V(semId);

    // Reimposta l'alarm per il prossimo STEP ATTIVATORE
    alarm(STEP_ATTIVATORE);
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
    statisticheSimulazione->attivazioniTotali++;
}
