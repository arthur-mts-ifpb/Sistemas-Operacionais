// Arthur Maurício

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define QTDE_CADEIRAS 1
#define QTDE_CLIENTES 3

const clock_t MAXDELAY = 2000000;

int clientes_esperando = 0; // Variável compartilhada
pthread_t barbeiro, clientes[QTDE_CLIENTES];
pthread_cond_t condBarbeiro; // Variável de semaforo

pthread_mutex_t the_mutex; //Mutex

void delay(clock_t ticks) { // Delay com espera ocupada
    clock_t start = clock();
    do
        ; while (clock() < start + ticks);
}

void corta_cabelo(int n) {
    printf("Cliente %d cortando cabelo\n", n);
    delay(MAXDELAY);
    printf("Cliente %d terminou de cortar cabelo\n", n);
}

void * cliente_t(int num_cliente) {
    pthread_mutex_lock(&the_mutex); // Entra na região crítica
    if (clientes_esperando < QTDE_CADEIRAS) {
        clientes_esperando++;
        printf("Cliente %d está esperando\n", num_cliente);

        pthread_cond_wait(&condBarbeiro, &the_mutex);
        printf("Cliente %d foi chamado\n", num_cliente);
        corta_cabelo(num_cliente);

    } else {
        // Sai da região crítica
        printf("Cliente %d saiu sem cortar cabelo\n", num_cliente);
    }
    pthread_mutex_unlock(&the_mutex);// Sai da região crítica
    pthread_exit(0);
}

void * barbeiro_t(void * n) {
    while(1) {
        pthread_mutex_lock(&the_mutex); // Barbeiro entra na região crítica
        if(clientes_esperando > 0) {
            clientes_esperando--;
            pthread_cond_signal(&condBarbeiro); // Barbeiro chama algum cliente
        }
        pthread_mutex_unlock(&the_mutex); // Barbeiro sai da região crítica
    }
    pthread_exit(0);
}

int main(int argc, char **argv)
{
    int e, st;
    pthread_attr_t attr; //data structure for managing thread attributes
    pthread_attr_init(&attr); //initialise thread attribute data ...
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    // Iniciando Mutex
    pthread_mutex_init(&the_mutex, NULL);

    // Iniciando semáforos
    pthread_cond_init(&condBarbeiro, 0);

    if (e = pthread_create(&barbeiro, &attr, barbeiro_t, NULL)!= 0) {
        printf("Thread create error %d\n", e);
        exit(-1);
    }

    for (int t=0; t<QTDE_CLIENTES; t++) { // create several worker thread instances
        //printf("Creating thread %d\n", t);

        if (e = pthread_create(&clientes[t], &attr, cliente_t, t)!= 0) {
            printf("Thread create error %d\n", e);
            exit(-1);
        }
    }
    
    pthread_attr_destroy(&attr); // Destrói o objeto de atributos

    pthread_join(barbeiro,(void **)&st);
    // Faz join em todas as threads
    for (int t=0; t<QTDE_CLIENTES; t++) { //wait for each worker thread to join
        if (e = pthread_join(clientes[t], (void **)&st)!= 0) {
            //printf("Thread %d join error %d\n", t, e);
            exit(-1);
        }
        //printf("Cliente %d saiu da barbearia: status = %x\n", t);
    }
    
    
    
    
}