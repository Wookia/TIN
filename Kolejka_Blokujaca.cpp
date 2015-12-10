#include "Kolejka_Blokujaca.h"
#include <queue>
#include "Struktura_danych.h"

std::queue<Struktura_danych> kolejka;
Kolejka_Blokujaca::Kolejka_Blokujaca()
{
    //inicjalizacja kolejki?
    sem_init(&semafor, 0, 1);
}

Kolejka_Blokujaca::~Kolejka_Blokujaca()
{
    //dtor
}

void Kolejka_Blokujaca::push(Struktura_danych dane)
{
    sem_wait(&semafor);
    kolejka.push(dane);
    //semafor w dol
    //czy sprawdzamy czy pelna? czy ustalamy jakis limit wielkosci kolejki?
    sem_post(&semafor);
    //semafor w gore
}

Struktura_danych Kolejka_Blokujaca::pop()
{
    sem_wait(&semafor);
    if(kolejka.size()==0)
    {
        sem_post(&semafor);
        //co zrobic jak sie nie uda? zwrocic nulla czy cos? ale to z zasady chyba powinno czekac az sie cos pojawi? ;/
    }
    else
    {
        Struktura_danych temp = kolejka.front();
        kolejka.pop();
        sem_post(&semafor);
        return temp;
    }


}
