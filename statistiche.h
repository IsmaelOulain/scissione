typedef struct
{
    int scissioniTotali;
    int scissioniUltimoSecondo;
    int attivazioniTotali;
    int attivazioniUltimoSecondo;
    int alimentazioniTotali;
    int alimentazioniUltimoSecondo;
    int scorieTotali;
    int scorieUltimoSecondo;
    long energia;
    // Aggiungi altri contatori o metriche secondo necessità
} StatisticheSimulazione;

// Questa variabile globale può essere utilizzata per accedere alla struttura condivisa
//StatisticheSimulazione *statisticheSimulazione;