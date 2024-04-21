#pragma once

// Definizione delle possibili cause di terminazione
typedef enum
{
    TIMEOUT_EXIT = 1,
    MELTDOWN_EXIT,
    BLACKOUT_EXIT,
    EXPLODE_EXIT
    // Altre cause di terminazione...
} ExitCause;

// Variabile globale per memorizzare la causa della terminazione
volatile ExitCause terminationCause = TIMEOUT_EXIT;

void createAtomo();

void waiting();

void cleanup();

void crea_messaggio();

void creaLista();

int creaSemaforo(const char *envVarName);

void stampaInfoAtomi();

void creaAttivatore();

void creaAlimentatore();

void simDuration();

void termination(int sig);

void waitAndStartSim();

void creaStatistiche();

void cleanupSM(int shmid, void *shmaddr);

void creaProcessoStampaStatistiche();

void stampaStatistiche();

void handleBlackout(int sig);

void handleMeltdown(int sig);

void handleExplode(int sig);

void setup_signal_handlers();
