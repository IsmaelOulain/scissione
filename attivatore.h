#pragma once
#include "messaggio_atomo.h"
int *arrayPid(int *selectedSize);

void stepAttivatore();
void comandoScissione(int signum);
void attendi_start();
void attendi_stop();
void start(int sig);
void stop(int sig);
void inviaMessaggio(TipoMessaggio tipo);
void stats();
void saveStats();
