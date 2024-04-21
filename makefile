# Definisce il compilatore
CC=gcc

# Definisce le opzioni di compilazione
CFLAGS= -Wvla -Wextra -Werror -D_GNU_SOURCE

# Nome degli eseguibili da generare
MASTER=master
ATOMO=atomo
ATTIVATORE=attivatore
ALIMENTATORE=alimentatore
STATISTICHE=statistiche

# Target predefinito che compila tutti gli eseguibili
all: $(MASTER) $(ATOMO) $(ATTIVATORE) $(ALIMENTATORE) $(STATISTICHE)

# Regola per compilare master
$(MASTER): master.c shared_array.c semaforo_binario.c
	$(CC) $(CFLAGS) master.c shared_array.c semaforo_binario.c -o $(MASTER) # Compila master.c, shared_array.c e semaforo_binario.c insieme

# Regola per compilare atomo
$(ATOMO): atomo.c shared_array.c semaforo_binario.c
	$(CC) $(CFLAGS) atomo.c shared_array.c semaforo_binario.c -o $(ATOMO) # Compila atomo.c, shared_array.c e semaforo_binario.c insieme

# Regola per compilare attivatore
$(ATTIVATORE): attivatore.c shared_array.c semaforo_binario.c
	$(CC) $(CFLAGS) attivatore.c shared_array.c semaforo_binario.c -o $(ATTIVATORE) # Compila attivatore.c, shared_array.c e semaforo_binario.c insieme

$(ALIMENTATORE): alimentatore.c shared_array.c semaforo_binario.c
	$(CC) $(CFLAGS) alimentatore.c shared_array.c semaforo_binario.c -o $(ALIMENTATORE) # Compila alimentatore.c, shared_array.c e semaforo_binario.c insieme

$(STATISTICHE): statistiche.c semaforo_binario.c
	$(CC) $(CFLAGS) statistiche.c  semaforo_binario.c -o statistiche  

# Pulizia dei file compilati
clean:
	rm -f $(MASTER) $(ATOMO) $(ATTIVATORE) $(ALIMENTATORE) $(STATISTICHE)

# Esegue il programma master
run: $(MASTER)
	./$(MASTER)
