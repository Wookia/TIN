#ifndef KOLEJKA_BLOKUJACA_H
#define KOLEJKA_BLOKUJACA_H
#include <queue>
#include <pthread.h>
#include <semaphore.h>
#include "Struktura_danych.h"

class Kolejka_Blokujaca
{
    public:
        Kolejka_Blokujaca();
        virtual ~Kolejka_Blokujaca();
        void push(Struktura_danych);
        Struktura_danych pop();
    protected:
    private:
        std::queue<Struktura_danych> kolejka;
        sem_t semafor;
};

#endif // KOLEJKA_BLOKUJACA_H
