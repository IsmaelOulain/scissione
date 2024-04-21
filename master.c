#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <string.h>
#include <errno.h>
#include <sys/sem.h>
#include <time.h>
#include <signal.h>
#include "master.h"
#include "messaggio_atomo.h"
#include "shared_array.h"
#include "semaforo_binario.h"
#include "statistiche.h"
#define N_ATOMI_INIT 2
#define N_ATOM_MAX 10 // Massimo valore per il numero atomico
#define SIM_DURATION 30
#define ENERGY_DEMAND 2
pid_t pid;
pid_t pidStats;
int msgid;
int shmid;
int shmidStats;
int semIdAtom;
int semIdStats;
SharedArray *sharedArray;
int pidAttivatore = 0;
int pidAlimentatore = 0;

// Questa variabile globale può essere utilizzata per accedere alla struttura condivisa
StatisticheSimulazione *statisticheSimulazione;

int main(){
    printf("Programma master iniziato PID: %d\n", getpid());
    printf("inizializzo coda messaggi atomi\n");
    char pidMasterStr[32];
    sprintf(pidMasterStr, "%d", getpid());
    setenv("PID_MASTER", pidMasterStr, 1);
    crea_messaggio();
    semIdAtom= creaSemaforo("SEMAFORO_ATOMI");
    semIdStats = creaSemaforo("SEMAFORO_STATISTICHE");
    setup_signal_handlers();
    creaLista();
    creaStatistiche();
    srand(time(NULL));

    createAtomo();
    stampaInfoAtomi();
    creaAttivatore();
    creaAlimentatore();
    waitAndStartSim();
    //TODO GESTIRE AVVIO SIMULAZIONE QUANDO PROCESSO ALIMENTATORE E ATTIVATORE SONO ON
    waiting();

    
    pause();
}

void setup_signal_handlers()
{
    struct sigaction sa;
    sa.sa_handler = handleBlackout;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGUSR1, &sa, NULL) == -1)
    {
        perror("Errore configurando sigaction per SIGUSR1");
        exit(EXIT_FAILURE);
    }

    // Configurazione di SIGTERM
    sa.sa_handler = handleMeltdown;
    if (sigaction(SIGUSR2, &sa, NULL) == -1)
    {
        perror("Errore configurando sigaction per SIGUSR1");
        exit(EXIT_FAILURE);
    }

    sa.sa_handler = handleExplode;
    if (sigaction(SIGINT, &sa, NULL) == -1)
    {
        perror("Errore configurando sigaction per SIGINT");
        exit(EXIT_FAILURE);
    }

    // // Configurazione di SIGUSR1
    // sa.sa_handler = handle_sigusr1;
    // sigaction(SIGUSR1, &sa, NULL);
}

void handleBlackout(int sig)
{
    printf("Ricevuto segnale di blackout. Terminazione di tutti i processi.\n");
    terminationCause= BLACKOUT_EXIT;
    termination(0);
}

void handleExplode(int sig){
    printf("Ricevuto segnale di explode. Terminazione di tutti i processi.\n");
    terminationCause = EXPLODE_EXIT;
    termination(0);
}

    void handleMeltdown(int sig)
{
    printf("Ricevuto segnale di Meltown. Terminazione di tutti i processi.\n");
    terminationCause = MELTDOWN_EXIT;
    termination(0);
}

void creaStatistiche()
{
    shmidStats = shmget(IPC_PRIVATE, sizeof(StatisticheSimulazione), IPC_CREAT | 0666);
    if (shmidStats < 0)
    {
        perror("shmget fallito");
        exit(1);
    }
    char shmidStats_str[20];
    sprintf(shmidStats_str, "%d", shmidStats);
    setenv("STATS_ID", shmidStats_str, 1);

    statisticheSimulazione = (StatisticheSimulazione *)shmat(shmidStats, NULL, 0);
    if (statisticheSimulazione == (void *)-1)
    {
        perror("shmat fallito");
        exit(1);
    }
    statisticheSimulazione->energia=10;
    // Inizializza i valori a zero
    //memset(statisticheSimulazione, 0, sizeof(StatisticheSimulazione));
}

void waitAndStartSim()
{
    int confermeRichieste = 2; 
    int confermeRicevute = 0;
    MessaggioAtomo msg;

    while (confermeRicevute < confermeRichieste)
    {
        msgrcv(msgid, &msg, sizeof(msg.info), 0, 0); 

        if (msg.mtype == CREAZIONE_ATTIVATORE || msg.mtype == CREAZIONE_ALIMENTATORE)
        {
            printf("Ricevuta conferma da %s\n", msg.mtype == CREAZIONE_ATTIVATORE ? "Attivatore" : "Alimentatore");
            confermeRicevute++;
        }
    }

    // Avvia sim
    simDuration();
}

void simDuration()
{
    printf("Avvio simulazione\n");
    //sleep(1);
    kill(pidAlimentatore, SIGUSR1);
    kill(pidAttivatore, SIGUSR1);
    signal(SIGALRM, termination);
    creaProcessoStampaStatistiche();
    alarm(SIM_DURATION);
    //creaProcessoStampaStatistiche();
}

int creaSemaforo(const char *envVarName)
{
    // Crea il semaforo con IPC_PRIVATE
    int semId = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
    if (semId == -1)
    {
        perror("semget");
        exit(EXIT_FAILURE);
    }

    char semId_str[20];
    sprintf(semId_str, "%d", semId);
    setenv(envVarName, semId_str, 1); 

    // Inizializzazione del Semaforo
    if (semctl(semId, 0, SETVAL, 1) == -1)
    { // Imposta il valore del semaforo a 1
        perror("semctl");
        exit(EXIT_FAILURE);
    }

    return semId; // Ritorna l'ID del semaforo creato per riferimenti futuri
}

void creaLista()
{
    // Creazione della memoria condivisa

    shmid = shmget(IPC_PRIVATE, sizeof(SharedArray), IPC_CREAT | 0666);
    if (shmid < 0)
    {
        perror("shmget");
        exit(1);
    }

    sharedArray = (SharedArray *)shmat(shmid, NULL, 0);
    if (sharedArray == (void *)-1)
    {
        perror("shmat");
        exit(1);
    }

    char shmid_str[20];
    sprintf(shmid_str, "%d", shmid);
    setenv("SHMPIDATOMI", shmid_str, 1);

    sharedArray->count = 0;

    // Aggiunge alcuni valori
}

void crea_messaggio()
{
    key_t key = IPC_PRIVATE;
    msgid = msgget(key, 0666 | IPC_CREAT);
    char msgIdStr[32];
    sprintf(msgIdStr, "%d", msgid);
    setenv("MSGATM_ID", msgIdStr, 1);
}

void waiting()
{
    int status;
    while ((pid = waitpid(-1, &status, WNOHANG)) != -1)
    {
        //printf("PARENT: PID=%d. Got info of child with PID=%d, status=0x%04X\n", getpid(), pid, status);
        // printf("PARENT: PID=%d. Got info of child with PID=%d, status=%d\n", getpid(), child_pid, WEXITSTATUS(status));
    }
    if (errno == ECHILD)
    {
        switch (terminationCause)
        {
        case TIMEOUT_EXIT:
            printf("Terminazione per timeout\n");
            break;
        case MELTDOWN_EXIT:
            printf("MELTDOWN: Terminazione di emergenza della simulazione.\n");
            break;
        case BLACKOUT_EXIT:
            printf("BLACKOUT: Terminazione di emergenza della simulazione.\n");
            break;
        case EXPLODE_EXIT:
            printf("EXPLODE: Terminazione di emergenza della simulazione.\n");
            break;
        }
        cleanup();
        exit(EXIT_SUCCESS);
    }
    else
    {
        fprintf(stderr, "Error #%d: %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }
}
void cleanup()
{
    if (semctl(semIdAtom, 0, IPC_RMID) == -1)
    {
        perror("semctl (rimozione)");
        exit(EXIT_FAILURE);
    }
    if (semctl(semIdStats, 0, IPC_RMID) == -1)
    {
        perror("semctl (rimozione)");
        exit(EXIT_FAILURE);
    }
    printf("pulito semafori\n");
    cleanupSM(shmid, sharedArray);// pulisco array dove registro i pid degli atomi
    printf("pulito array pid atomi\n");
    cleanupSM(shmidStats, statisticheSimulazione);// pulisco struttura dove mantengo le statistiche
    printf("pulito struttura statistiche\n");
}

void cleanupSM(int shmid, void *shmaddr)
{

    if (shmdt(shmaddr) == -1)
    {
        perror("shmdt");
        exit(EXIT_FAILURE);
    }

    if (shmctl(shmid, IPC_RMID, NULL) == -1)
    {
        perror("shmctl IPC_RMID");
        exit(EXIT_FAILURE);
    }
}

void createAtomo()
{
    int flag_master=1;
    for (int i = 0; i < N_ATOMI_INIT; i++)
    {
        //Genero un numero atomico casuale tra 1 e N_ATOM_MAX
        int atomicNumber = (rand() % N_ATOM_MAX) + 1;
        
        char atomicNumberStr[10];
        char flag_masterStr[2];
        sprintf(atomicNumberStr, "%d", atomicNumber);
        sprintf(flag_masterStr, "%d", flag_master);
        switch (pid = fork())
        {
        case -1:
            terminationCause = MELTDOWN_EXIT;
            termination(0);
            break;
        case 0:
            execlp("./atomo", "atomo", atomicNumberStr, flag_masterStr, NULL);
            break;

        default:
            break;
        }
    }
}

void stampaInfoAtomi()
{
    int num_figli_attesi = 0;

    while (num_figli_attesi < N_ATOMI_INIT)
    {
        MessaggioAtomo msg;
        if (msgrcv(msgid, &msg, sizeof(msg.info), 0, 0) > 0)
        {
            switch (msg.mtype)
            {
            case CREAZIONE_ATOMO:
                printf("LOG master.c: Atomo creato: PID %d, PPID %d, Numero Atomico %d\n",
                       msg.info.pid, msg.info.ppid, msg.info.numAtomico);
                num_figli_attesi++;
                break;
            }
        }

        sleep(1);
    }
}

void creaAttivatore(){
    pidAttivatore = fork();
    if (pidAttivatore == -1)
    {
        terminationCause = MELTDOWN_EXIT;
        termination(0);
    }
    else if (pidAttivatore == 0)
    {
        // Esegui il programma dell'attivatore
        execlp("./attivatore", "attivatore", (char *)NULL);
        fprintf(stderr, "Fallimento nell'esecuzione di execlp per l'attivatore: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    else
    {

    }
}

void creaAlimentatore()
{
    pidAlimentatore = fork();
    if (pidAlimentatore == -1)
    {
        terminationCause = MELTDOWN_EXIT;
        termination(0);
    }
    else if (pidAlimentatore == 0)
    {
        // Esegui il programma dell'attivatore
        execlp("./alimentatore", "alimentatore", (char *)NULL);
        fprintf(stderr, "Fallimento nell'esecuzione di execlp per l'alimentatore: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    else
    {
    
    }
}

void termination(int sig)
{
    kill(pidStats, SIGTERM);
    P(semIdAtom);

    if (!isEmpty(sharedArray))
    {
       
        for (int i = 0; i < sharedArray->count; i++)
        {
            kill(sharedArray->data[i], SIGTERM);
            //printf("Inviato SIGTERM a PID %d\n", sharedArray->data[i]);
        }
    }
    printf("Tutti gli atomi sono stati terminati\n");

    V(semIdAtom);


    if (pidAttivatore > 0)
    {
        kill(pidAttivatore, SIGTERM);
        printf("Inviato segnale terminazione all'Attivatore\n");
    }

    if (pidAlimentatore > 0)
    {
        kill(pidAlimentatore, SIGTERM);
        printf("Inviato segnale terminazione all'Alimentatore\n");
    }

    //printf("tot:%d\n",statisticheSimulazione->scissioniTotali);
    waiting();
    printf("Simulazione terminata Master termina.\n");
    exit(EXIT_SUCCESS);
}


void creaProcessoStampaStatistiche()
{

    char energyDemandStr[10];
    sprintf(energyDemandStr, "%d", ENERGY_DEMAND);
    char pidMasterStr[10];
    sprintf(pidMasterStr, "%d", getpid());
    pidStats = fork();
    if (pidStats == -1)
    {
        perror("Errore nella fork del processo di stampa");
        exit(EXIT_FAILURE);
    }
    else if (pidStats == 0)
    {
        execlp("./statistiche", "statistiche", energyDemandStr, pidMasterStr, NULL);
        fprintf(stderr, "Fallimento nell'esecuzione di execlp per il processo di stampa: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
}


