#include "LifespanQueue.h"
#include <unistd.h>

using namespace std;

#define TYPE int

LifespanQueue<TYPE> *lq;

void *THREAD(void *args)
{
    usleep((rand() % 100000) + 100000);
    int mex = rand() % 10;
    cout << "Provo ad inserire il messaggio: " << mex << endl;
    lq->push(mex);

    lq->printQueue();

    usleep(500000 - (rand() % 10000));

    //cout << "Ci arrivo" << endl;
    int ret = lq->pop();
    cout << "Messaggio prelevato: " << ret << endl;

    lq->printQueue();

    usleep((rand() % 100000) + 300000);

    exit(EXIT_SUCCESS);
}

int main()
{
    int n_thread;
    cout << "Insert threads number: ";
    cin >> n_thread;
    cout << endl;

    int queue_size;
    cout << "Insert queue size: ";
    cin >> queue_size;
    cout << endl;

    float expiration_time;
    cout << "Insert message expiration time: ";
    cin >> expiration_time;
    cout << endl;

    lq = new LifespanQueue<TYPE>(queue_size, expiration_time);

    pthread_t *p;
    //pthread_attr_t a;
    srand(555);

    //pthread_attr_init(&a);
    p = (pthread_t *)malloc(n_thread * sizeof(pthread_t));

    for(int i = 0; i < n_thread; i++) {
        pthread_create(&p[i], NULL, THREAD, NULL);
        usleep(100);
    }

    //pthread_attr_destroy(&a);

    for(int i = 0; i < n_thread; i++) {
        pthread_join(p[i], NULL);
    }

    free(p);

    return 0;
}