#include "SynchronizedQueue.h"
#include <queue>

template<class T>
SynchronizedQueue<T>::SynchronizedQueue()
{
    //inicjalizacja kolejki?
    sem_init(&sem, 0, 1);
}

template<class T>
SynchronizedQueue<T>::~SynchronizedQueue()
{
    sem_destroy(&sem);
}

template<class T>
void SynchronizedQueue<T>::push(T& dane)
{
    sem_wait(&sem);
    kolejka.push(dane);
    //semafor w dol
    //czy sprawdzamy czy pelna? czy ustalamy jakis limit wielkosci kolejki?
    sem_post(&sem);
    //semafor w gore
}

template<class T>
T& SynchronizedQueue<T>::pop()
{
    sem_wait(&sem);
    if(kolejka.size()==0)
    {
        sem_post(&sem);
        return NULL;
        //co zrobic jak sie nie uda? zwrocic nulla czy cos? ale to z zasady chyba powinno czekac az sie cos pojawi? ;/
    }
    else
    {
        T temp = kolejka.front();
        kolejka.pop();
        sem_post(&sem);
        return temp;
    }


}
