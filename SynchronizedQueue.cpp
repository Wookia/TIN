#include "SynchronizedQueue.h"
#include <queue>
#include <iostream>

template<class T>
SynchronizedQueue<T>::SynchronizedQueue()
{
    //inicjalizacja kolejki?
    sem_init(&sem, 0, 1);
    sem_init(&sem_count, 0, 0);
}

template<class T>
SynchronizedQueue<T>::~SynchronizedQueue()
{
    sem_destroy(&sem);
    sem_destroy(&sem_count);
}

template<class T>
void SynchronizedQueue<T>::push(T& dane)
{
    sem_wait(&sem);
    kolejka.push(dane);
    //semafor w dol
    //czy sprawdzamy czy pelna? czy ustalamy jakis limit wielkosci kolejki?
    sem_post(&sem_count);
    sem_post(&sem);
    //semafor w gore
}

template<class T>
T& SynchronizedQueue<T>::pop()
{
	sem_wait(&sem_count);
    sem_wait(&sem);
    if(kolejka.size()==0) //to jest useless ale niech bedzie
    {
		std::cout << "Niedopuszczalny stan! poszedl semafor a pusta kolejka!" << std::endl;
		perror("semafor ilosci elementow przepuscil a jest pusta kolejka!");
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
