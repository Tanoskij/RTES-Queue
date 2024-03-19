#include "LifespanQueue.h"
#include <unistd.h>
#include <string.h>

using namespace std;

#define TYPE int
//#define TYPE char

LifespanQueue<TYPE> *lq;

void *WRITER(void *args)
{
    for(int i = 0; i < 10; i++) {
        usleep((rand() % 100000) + 100000);
        int mex = rand() % 10;
        //char mex = 'c';

        cout << "Provo ad inserire il messaggio: " << mex << endl;
        lq->push(mex);

        lq->printQueue();

        usleep(500000 - (rand() % 10000));
    }

    return NULL;
}

void *READER(void *args)
{
    for (int i = 0; i < 10; i++) {
        usleep(500000 - (rand() % 10000));
        int ret = lq->bPop();
        cout << "Messaggio prelevato: " << ret << endl;

        lq->printQueue();

        usleep((rand() % 100000) + 300000);
    }

    return NULL;
}

int main()
{
    char n_writers[20];

    while(1) {
        int i;

        printf("Enter the writers number: ");
        fgets(n_writers, 20, stdin);

        for(i = 0; i < (strlen(n_writers) - 1); i++)
            if(!isdigit(n_writers[i])) {
                printf("Attention: incorrect number entered.\n");
                break;
            }
        
        if(i == 0)
            continue;

        if(i == strlen(n_writers) - 1)
            if(atoi(n_writers) < 1)
                printf("Minimum writers count: 1.\n");
            else
                break;
    }

    char n_readers[20];

    while(1) {
        int i;

        printf("Enter the readers number: ");
        fgets(n_readers, 20, stdin);

        for(i = 0; i < (strlen(n_readers) - 1); i++)
            if(!isdigit(n_readers[i])) {
                printf("Attention: incorrect number entered.\n");
                break;
            }
        
        if(i == 0)
            continue;

        if(i == strlen(n_readers) - 1)
            if(atoi(n_readers) < 1)
                printf("Minimum readers count: 1.\n");
            else
                break;
    }

    char queue_size[20];

    while(1) {
        int i;

        printf("Enter the queue dimension: ");
        fgets(queue_size, 20, stdin);

        for(i = 0; i < (strlen(queue_size) - 1); i++)
            if(!isdigit(queue_size[i])) {
                printf("Attention: incorrect dimension entered.\n");
                break;
            }
        
        if(i == 0)
            continue;

        if(i == strlen(queue_size) - 1)
            if(atoi(queue_size) < 1)
                printf("Minimum queue size.\n");
            else
                break;
    }

    char expiration_time[20];

    while(1) {
        int i;

        printf("Enter the message expiration time (milliseconds): ");
        fgets(expiration_time, 20, stdin);

        for(i = 0; i < (strlen(expiration_time) - 1); i++)
            if(!isdigit(expiration_time[i])) {
                printf("Attention: incorrect expiration time entered.\n");
                break;
            }
        
        if(i == 0)
            continue;

        if(i == strlen(expiration_time) - 1)
            if(atoi(expiration_time) < 1)
                printf("Minimum expiration time: 1ms.\n");
            else
                break;
    }

    lq = new LifespanQueue<TYPE>(atoi(queue_size), atoi(expiration_time));

    pthread_t *p;
    srand(555);

    p = (pthread_t *)malloc((atoi(n_writers) + atoi(n_readers)) * sizeof(pthread_t));

    for(int i = 0; i < atoi(n_writers); i++) {
        pthread_create(&p[i], NULL, WRITER, NULL);
        usleep(100);
    }

    for(int i = atoi(n_writers); i < (atoi(n_writers) + atoi(n_readers)); i++) {
        pthread_create(&p[i], NULL, READER, NULL);
        usleep(100);
    }

    for(int i = 0; i < atoi(n_writers); i++) {
        pthread_join(p[i], NULL);
    }

    free(p);

    return 0;
}