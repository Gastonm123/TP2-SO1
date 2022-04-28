#include <stdio.h>
#include <pthread.h>
#include <math.h>
#include "lamport.h"

#define N_VISITANTES 1000000
#define N_THREADS 4

int visitantes = 0;
int numero[N_THREADS], eligiendo[N_THREADS];

void printf_numero()
{
  printf("numero: ");
  for (int i = 0; i < N_THREADS; i++)
    printf("%d ", numero[i]);
  printf("\n");
}

void lock_visitantes(void *arg) {
  int id = arg - (void *) NULL;
  lock(id, N_THREADS, numero, eligiendo);
}

void unlock_visitantes(void *arg) {
  int id = arg - (void *) NULL;
  unlock(id, numero);
}

void *molinete(void *arg)
{
  int i;
  for (i=0;i<N_VISITANTES;i++){
    lock_visitantes(arg);
    visitantes++;
    unlock_visitantes(arg);
  }
}

int main()
{
  pthread_t threads[N_THREADS];

  for (int i = 0; i < N_THREADS; i++) {
    eligiendo[i] = 0;
    numero[i] = 0;
  }

  for (int i = 0; i < N_THREADS; i++) {
    pthread_create(&threads[i], NULL, molinete, NULL + i);
  }

  for (int i = 0; i < N_THREADS; i++) {
    pthread_join(threads[i], NULL);
  }

  printf("Hoy hubo %d visitantes!\n", visitantes);
  return 0;
}
