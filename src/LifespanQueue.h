#ifndef LIFESPANQUEUE_H
#define LIFESPANQUEUE_H

#include <iostream>
#include <stdlib.h>
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

        // Contatori indicanti il numero di thread bloccati in attesa di inserire/estrarre dalla coda
        int blocked_push, blocked_pop;

        // Funzione controlla se vi sono messaggi scaduti nella coda
        bool expired(void);

    public:
        // Costruttore
        LifespanQueue(size_t dim, int expiration_time) {
            if(dim < 1)
                throw logic_error("Invalid dimension");
            
            if(expiration_time <= 0)
                throw logic_error("Invalid expiration time");
            
            this->dim = dim;

            // Converto l'expiration time da ms a ns
            this->expiration_time = expiration_time * 1000;

            this->queue = (struct element<T> *)malloc(this->dim * sizeof(struct element<T>));

            this->head = this->tail = 0;
            this->count = 0;
            
            sem_init(&this->mutex, 0, 1);
            sem_init(&this->push_sem, 0, dim);
            sem_init(&this->pop_sem, 0, 0);

            this->blocked_push = this->blocked_pop = 0;
        }

        // Inserisce un nuovo elemento nella coda (bloccante)
        void bPush(T message) {
            sem_wait(&this->mutex);

            while(full() && !expired()) {
                this->blocked_push++;
                sem_post(&this->mutex);
                sem_wait(&this->push_sem);
                sem_wait(&this->mutex);
            }

            this->queue[this->head]->message = message;
            this->queue[this->head]->time = chrono::steady_clock::now();
            this->head = (this->head + 1) % this->dim;
            this->count++;

            if(this->blocked_pop) {
                this->blocked_pop--;
                sem_post(&this->pop_sem);
            }

            sem_post(&this->mutex);
        }
        
        // Inserisce un nuovo elemento nella coda (non bloccante)
        void push(T message) {
            sem_wait(&this->mutex);

            if(full() && !expired()) {
                cout << "Queue is full." << endl;
                return;
            }

            this->queue[this->head]->message = message;
            this->queue[this->head]->time = chrono::steady_clock::now();
            this->head = (this->head + 1) % this->dim;
            this->count++;

            if(this->blocked_pop) {
                this->blocked_pop--;
                sem_post(&this->pop_sem);
            }

            sem_post(&this->mutex);
        }

        // Preleva un elemento dalla coda
        T pop(void);

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