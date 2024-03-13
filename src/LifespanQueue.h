#ifndef LIFESPANQUEUE_H
#define LIFESPANQUEUE_H

#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <chrono>

using namespace std;

// Definizione della struttura dei messaggi nella coda
template <typename T> struct element {
    T message;
    chrono::steady_clock::time_point time;
};

template <class T> class LifespanQueue {
    private:
        // Coda dei messaggi
        struct element<T> *queue;

        // Dimensione della coda
        size_t dim;

        // Indici per aggiungere/togliere un elemento dalla coda
        size_t head, tail;

        // Numero di elementi presenti nella coda
        size_t count;

        // Tempo di scadenza del messaggio
        int expiration_time;

        // Semaforo di mutua esclusione per l'accesso alla coda
        sem_t mutex;

        // Semafori per gestire l'inserimento o l'estrazione di elementi nella coda
        sem_t push_sem, pop_sem;
    
    public:
        // Costruttore
        LifespanQueue(size_t dim, int expiration_time);

        void push(struct element<T>);   // Inserisce un nuovo elemento nella coda
        struct element<T> pop(void);    // Preleva un elemento dalla coda

        // Controlla se la coda è vuota
        bool empty(void) {
            return (this->count == 0) ? true : false;
        }

        // Controlla se la coda è piena
        bool full(void) {
            return (this->count == this->dim) ? true : false;
        }
};

#endif