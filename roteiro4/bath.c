// Arthur Maurício

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// Definindo tipos e estado
#define BOXES 3
#define PEOPLE 6

#define EMPTY 0
#define WOMAN 1
#define MAN 2

typedef int STATE;

// Variáveis compartilhadas
STATE bathroom_state = EMPTY;

int OCCUPIED_BOXES = 0;

// Definindo mutex e semaforos
pthread_mutex_t bathroom;

pthread_t people_threads[PEOPLE];

// pthread_cond_t
pthread_cond_t woman_leaves, man_leaves, empty_bathroom;

void delay()
{ // Delay com espera ocupada
  clock_t start = clock();
  clock_t ticks = rand() % 2000000;
  do
    ;
  while (clock() < start + ticks);
}

// Banheiros
void man_try_to_enter(int  n) {
  pthread_mutex_lock(&bathroom);
  switch (bathroom_state)
  {
  case MAN:
    if(OCCUPIED_BOXES < BOXES) {
      OCCUPIED_BOXES++;
    } 
    else{
      printf("Homem %d esperando na porta...\n", n);
      pthread_cond_wait(&man_leaves, &bathroom);
    }
    break;
  case WOMAN:
    printf("Homem %d esperando na porta...\n", n);
    pthread_cond_wait(&empty_bathroom, &bathroom);
    break;
  default:
    bathroom_state = MAN;
    OCCUPIED_BOXES++;
    break;
  }
  printf("Homem %d ocupando box...\n", n);
  pthread_mutex_unlock(&bathroom);
}

void man_exit(int n) {
  pthread_mutex_lock(&bathroom);
  OCCUPIED_BOXES--;
  if(OCCUPIED_BOXES == 0) pthread_cond_broadcast(&empty_bathroom);
  else pthread_cond_signal(&man_leaves);
  pthread_mutex_unlock(&bathroom);
}

void * man(int n) {
  printf("Homem %d na porta\n", n);
  delay();
  man_try_to_enter(n);
  delay();
  printf("Homem %d saindo\n", n);
  man_exit(n);
  pthread_exit(0);
}

void woman_try_to_enter(int n) {
  pthread_mutex_lock(&bathroom);
  switch (bathroom_state)
  {
  case WOMAN:
    if(OCCUPIED_BOXES < BOXES) 
      OCCUPIED_BOXES++;
    else{
      printf("Mulher %d esperando na porta...\n", n);
      pthread_cond_wait(&woman_leaves, &bathroom);
    }
    break;
  case MAN:
    printf("Mulher %d esperando na porta...\n", n);
    pthread_cond_wait(&empty_bathroom, &bathroom);
    break;
  default:
    bathroom_state = WOMAN;
    OCCUPIED_BOXES++;
    break;
  }
  printf("Mulher %d ocupando box...\n", n);
  pthread_mutex_unlock(&bathroom);
}

void woman_exit(int n) {
  pthread_mutex_lock(&bathroom);
  OCCUPIED_BOXES--;
  if(OCCUPIED_BOXES == 0) pthread_cond_broadcast(&empty_bathroom);
  else pthread_cond_signal(&woman_leaves);
  pthread_mutex_unlock(&bathroom);
}

void * woman(int n) {
  printf("Mulher %d na porta\n", n);
  delay();
  woman_try_to_enter(n);
  delay();
  printf("Mulher %d saindo\n", n);

  woman_exit(n);
  pthread_exit(0);
}

int main(int argc, char **argv)
{
  pthread_mutex_init(&bathroom, NULL);

  pthread_cond_init(&woman_leaves,NULL);
  pthread_cond_init(&man_leaves, NULL);
  pthread_cond_init(&empty_bathroom, NULL);


  srand(time(NULL)); 

  for (int t = 0; t < PEOPLE; t++)
  {
    if (t % 2 == 0)
      pthread_create(&people_threads[t], NULL, woman, t);
    else
      pthread_create(&people_threads[t], NULL, man, t);
  }

  for(int t = 0; t < PEOPLE; t++) 
    pthread_join(people_threads[t], NULL);

  pthread_mutex_destroy(&bathroom);
}
