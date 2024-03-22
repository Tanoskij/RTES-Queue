# RTES-Queue (Real-Time Embedded Systems project)
Coda per messaggi thread-safe che adotta una politica QoS Lifespan.

L'inserimento di un messaggio nella coda coincide con l'attivazione di un timer (uno per messaggio inserito). Se il tempo trascorso all'interno della coda è maggiore del tempo di scadenza (fissato dall'utente), il messaggio verrà eliminato dalla coda.
La coda possiede metodi che permettono di sfruttare l'inserimento/estrazione dei messaggi da parte dei vari thread in modo bloccante, misto o non bloccante.

## File di test
All'interno della repository sono presenti 3 file di test che mostrano come istanziare la coda ed il funzionamento dei metodi presenti al suo interno:
* Test.cpp: implementazione con push e pop non bloccanti;
* Test_b.cpp: implementazione con push e pop bloccanti;
* Test_mixed.cpp: implementazione mista.

## Esecuzione
Per la prima esecuzione:
```bash
cd src
make
```
Gli eseguibili saranno inseriti nella sottodirectory bin. Quindi:
```bash
cd bin
./[fileName].o
```

## Slide
All'interno della repository è possibile trovare delle slide che illustrano variabili e metodi utilizzati per la creazione della coda.
