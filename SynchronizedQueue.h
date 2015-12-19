#ifndef synchronized_queue_h_
#define synchronized_queue_h_
#include <queue>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>

template <class T>
class SynchronizedQueue
{
    public:
        SynchronizedQueue();
        virtual ~SynchronizedQueue();
        void push(T&);
        T& pop();
    protected:
    private:
        std::queue<T> kolejka;
        sem_t sem;
};

#endif
