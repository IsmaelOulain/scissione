// #include <stdio.h>
// #include <stdlib.h>
// #include <sys/shm.h>
// #include <time.h>
// #include <unistd.h>
// #include <signal.h>
#include "utils.h"
#include "shared_array.h"
#include "semaforo_binario.h"
#include "statistiche.h"

int main(int argc, char *argv[])
{
    int energia_demand = atoi(argv[1]);
    int pidMaster = atoi(argv[2]);

    int semIdStats = atoi(getenv("SEMAFORO_STATISTICHE"));
    int shmidStats = atoi(getenv("STATS_ID"));

    // Collegamento alla memoria condivisa
    StatisticheSimulazione *statisticheSimulazione = shmat(shmidStats, NULL, 0);
    if (statisticheSimulazione == (void *)-1)
    {
        perror("shmat fallito");
        exit(EXIT_FAILURE);
    }
    struct timespec intervallo;
    intervallo.tv_sec = 1;  // Intervallo di un secondo
    intervallo.tv_nsec = 0; 

    while (1)
    {
        nanosleep(&intervallo, NULL);

        P(semIdStats);
        printf("Scissioni Totali: %d, ", statisticheSimulazione->scissioniTotali);
        printf("Attivazioni Totali: %d, ", statisticheSimulazione->attivazioniTotali);
        printf("Alimentazioni Totali: %d ", statisticheSimulazione->alimentazioniTotali);
        printf("Scorie Totali: %d ", statisticheSimulazione->scorieTotali);
        printf("Energia Totale: %ld\n", statisticheSimulazione->energia);

        if (statisticheSimulazione->energia < ENERGY_EXPLODE_THRESHOLD)
        {
            if (statisticheSimulazione->energia >= energia_demand)
            {
                statisticheSimulazione->energia -= energia_demand;
            }
            else
            {
                printf("Blackout: energia insufficiente!\n");
                V(semIdStats);
                kill(pidMaster, SIGUSR1); // Invia segnale di blackout al master
                exit(EXIT_SUCCESS);
            }
        }else{
            printf("Explode: energia supera il threshold!\n");
            V(semIdStats);
            kill(pidMaster, SIGINT); // Invia segnale di explode al master
            exit(EXIT_SUCCESS);
        }

        

        V(semIdStats);
    }
}
