#include "SynchronizedQueue.h"
#include <queue>
#include <iostream>
#include "Packet.h"
#include <list>

template class SynchronizedQueue<Packet>;
template class SynchronizedQueue<std::list<Packet>>;

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
void SynchronizedQueue<T>::push(T dane)
{
	std::cout << "Wstawianie do kolejki" << std::endl;
    sem_wait(&sem);
    kolejka.push(dane);
    //semafor w dol
    //czy sprawdzamy czy pelna? czy ustalamy jakis limit wielkosci kolejki?
    sem_post(&sem_count);
    sem_post(&sem);
    std::cout << "Wstawiono!" << std::endl;
    //semafor w gore
}

template<class T>
T SynchronizedQueue<T>::pop()
{
	std::cout << "Wyciaganie z kolejki" << std::endl;
	sem_wait(&sem_count);
    sem_wait(&sem);
    if(kolejka.size()==0) //to jest useless ale niech bedzie
    {
		std::cout << "Niedopuszczalny stan! poszedl semafor a pusta kolejka!" << std::endl;
        sem_post(&sem);
        //co zrobic jak sie nie uda? zwrocic nulla czy cos? ale to z zasady chyba powinno czekac az sie cos pojawi? ;/
    }
    else
    {
        T temp = kolejka.front();
        kolejka.pop();
        sem_post(&sem);
        std::cout << "Wyciagnieto!" << std::endl;
        return temp;
    }


}
