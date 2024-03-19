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

        // Funzione che controlla se vi sono messaggi scaduti nella coda e ritorna se ne sono stati rimossi
        bool expired(void) {
            int previous_tail = this->tail;

            for(int index = this->tail; this->count > 0; index = (index + 1) % this->dim) {

                if((chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - this->queue[index].time).count()) < this->expiration_time)
                    break;
                else {
                    this->tail = (this->tail + 1) % this->dim;
                    this->count--;
                }
            }

            return (this->tail == previous_tail) ? false : true;
        }

    public:
        // Costruttore
        LifespanQueue(size_t dim, int expiration_time) {
            if(dim < 1)
                throw logic_error("Invalid dimension.");
            
            if(expiration_time <= 0)
                throw logic_error("Invalid expiration time.");
            
            this->dim = dim;

            // Converto l'expiration time da ms a ns
            this->expiration_time = expiration_time;

            this->queue = (struct element<T> *)malloc(this->dim * sizeof(struct element<T>));

            this->head = this->tail = 0;
            this->count = 0;
            
            sem_init(&this->mutex, 0, 1);
            sem_init(&this->push_sem, 0, 0);
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

            this->queue[this->head].message = message;
            this->queue[this->head].time = chrono::steady_clock::now();
            this->head = (this->head + 1) % this->dim;
            this->count++;

            for(int i = 0; i < this->count && this->blocked_pop; i++) {
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
                sem_post(&this->mutex);
                return;
            }

            this->queue[this->head].message = message;
            this->queue[this->head].time = chrono::steady_clock::now();
            this->head = (this->head + 1) % this->dim;
            this->count++;

            for(int i = 0; i < this->count && this->blocked_pop; i++) {
                this->blocked_pop--;
                sem_post(&this->pop_sem);
            }

            sem_post(&this->mutex);
        }

        // Preleva un elemento dalla coda (bloccante)
        T bPop(void) {
            sem_wait(&this->mutex);

            T message;

            while(1) {
                expired();

                if(!empty()) {
                    message = this->queue[this->tail].message;
                    this->tail = (this->tail + 1) % this->dim;
                    this->count--;
                    break;
                }
                else {
                    this->blocked_pop++;
                    sem_post(&this->mutex);
                    sem_wait(&this->pop_sem);
                    sem_wait(&this->mutex);
                }
            }

            for(int i = 0; i < (this->dim - this->count) && this->blocked_push; i++) {
                this->blocked_push--;
                sem_post(&this->push_sem);
            }

            sem_post(&this->mutex);
            return message;
        }

        // Preleva un elemento dalla coda (non bloccante)
        T pop(void) {
            sem_wait(&this->mutex);

            expired();

            if(empty()) {
                cout << "Queue is empty." << endl;
                sem_post(&this->mutex);
                return NULL;
            }

            T message = this->queue[this->tail].message;

            this->tail = (this->tail + 1) % this->dim;
            this->count--;

            for(int i = 0; i < (this->dim - this->count) && this->blocked_push; i++) {
                this->blocked_push--;
                sem_post(&this->push_sem);
            }

            sem_post(&this->mutex);
            return message;
        }

        // Controlla se la coda è vuota
        bool empty(void) {
            return (this->count == 0) ? true : false;
        }

        // Controlla se la coda è piena
        bool full(void) {
            return (this->count == this->dim) ? true : false;
        }

        // Stampa gli elementi attuali che compongono la coda
        void printQueue(void) {
            sem_wait(&this->mutex);

            int index = this->tail;

            if(empty())
                cout << "Impossible to print the queue. Queue is empty." << endl << endl;
            else
                for(int i = 0; i < this->count; i++) {
                    cout << "Elem. n. " << (i + 1) << ":" << endl;
                    cout << "Message: " << this->queue[index].message << " | ";
                    cout << "Elapsed time: " << chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - this->queue[index].time).count() << endl;
                    cout << endl;
                    index = (index + 1) % this->dim;
                }
            
            sem_post(&this->mutex);
        }
};

#endif