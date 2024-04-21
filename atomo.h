#pragma once
#define MIN_NUM_ATOM 2
void inviaMessaggio(TipoMessaggio tipo);
void savePid();
void attendi_comando();
void gestoreScissione(int sig);
void createAtomo();
void saveStats(int scoria);
long energy(int n1, int n2);