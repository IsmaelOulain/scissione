// #include <stdio.h>
// #include <unistd.h>
// #include <stdlib.h>
// #include <signal.h>
// #include <sys/shm.h>
// #include <sys/sem.h>
// #include <time.h>
// #include <sys/msg.h>
#include "utils.h"
#include "messaggio_atomo.h"
#include "shared_array.h"
#include "semaforo_binario.h"
#include "atomo.h"
#include "statistiche.h"
#define EXIT_SCORIA 2

SharedArray *sharedArray;
StatisticheSimulazione *statisticheSimulazione;
int semId;
int semIdStats;
int shmid;
int shmidStats;
int flag_master;
int num_atomico;
int atomicNumberChild;
int pidMaster;
int main(int argc, char *argv[])
{
    if (argc < 1)
    {
        fprintf(stderr, "Uso: %s <numero_atomico>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    semId = atoi(getenv("SEMAFORO_ATOMI"));
    pidMaster = atoi(getenv("PID_MASTER"));
    semIdStats = atoi(getenv("SEMAFORO_STATISTICHE"));
    shmid = atoi(getenv("SHMPIDATOMI"));
    shmidStats = atoi(getenv("STATS_ID"));

    num_atomico = atoi(argv[1]);
    flag_master = atoi(argv[2]);
    srand(time(NULL));

    if(flag_master==1){
        inviaMessaggio(CREAZIONE_ATOMO);
        
    }
    

    P(semId);

    savePid();

    V(semId);

    //printSharedArray(sharedArray);

    attendi_comando();
}

void savePid()
{
    
    if (shmid == 0)
    {
        fprintf(stderr, "SHMID non impostato.\n");
        exit(1);
    }

    sharedArray = (SharedArray *)shmat(shmid, NULL, 0);
    if (sharedArray == (void *)-1)
    {
        perror("shmat");
        exit(1);
    }
    push(sharedArray, getpid());
}

void saveStats(int scoria)
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

    if(scoria!=1){
        //printf("no scoria \n");
        // printf("atomo tot %d\n", statisticheSimulazione->scissioniTotali);
        statisticheSimulazione->scissioniTotali++;
        int en = energy(num_atomico, atomicNumberChild);
        // printf("energia prima %ld\n", statisticheSimulazione->energia);
        statisticheSimulazione->energia += en;
        // printf("Padre atom:%d , figlio atom:%d, energia dopo %ld\n", num_atomico, atomicNumberChild, statisticheSimulazione->energia);
    }else{
        //printf("scoria \n");
        statisticheSimulazione->scorieTotali++;
    }
    
}

void inviaMessaggio(TipoMessaggio tipo)
{
    char *msgIdStr = getenv("MSGATM_ID");
    int msg_id = atoi(msgIdStr);
    MessaggioAtomo msg;
    msg.mtype = tipo;
    msg.info.pid = getpid();
    msg.info.ppid = getppid();
    msg.info.numAtomico=num_atomico;

    msgsnd(msg_id, &msg, sizeof(msg.info), 0);
    //exit(EXIT_SUCCESS);
}

void attendi_comando(){
    struct sigaction sa;
    sa.sa_handler = gestoreScissione; // Imposta la funzione di gestione del segnale
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    // Configura il gestore per SIGUSR2
    if (sigaction(SIGUSR2, &sa, NULL) == -1)
    {
        perror("Errore in sigaction");
        exit(EXIT_FAILURE);
    }
    while (1)
    {
        pause(); // Aspetta il segnale di scissione
    }
}

void gestoreScissione(int sig)
{
    //int scoria=0;
    
    if (num_atomico < MIN_NUM_ATOM)
    {
        P(semIdStats);
        saveStats(1);
        V(semIdStats);

        P(semId);
        pop(sharedArray, getpid());
        V(semId);

        
        exit(EXIT_SCORIA);
    }else{
        createAtomo();
      
        
    }   
    //exit(EXIT_SUCCESS);
}

void createAtomo()
{
   
        // Genero un numero atomico casuale tra 1 e N_ATOM_MAX
        atomicNumberChild = num_atomico/2;
        char atomicNumberStr[10];
        sprintf(atomicNumberStr, "%d", atomicNumberChild);
        int pid=0;
        num_atomico = num_atomico - atomicNumberChild;
        
        switch (pid = fork())
        {
        case -1:
            //printf("MELTDOWN");
            kill(pidMaster,SIGUSR2);
            exit(EXIT_SUCCESS);
            break;
        case 0:
            // printf("creato %d\n",getpid());
            /* Perform actions specific to child */
            P(semIdStats);
            saveStats(0);
            V(semIdStats);
            execlp("./atomo", "atomo", atomicNumberStr, "0", NULL);
            break;

        default:

            break;
        }

    //waiting();
}

int max(int a, int b)
{
    return (a > b) ? a : b;
}

// Funzione per calcolare l'energia liberata durante la scissione di un atomo
long energy(int n1, int n2)
{
    return (n1 * n2) - max(n1, n2);
}