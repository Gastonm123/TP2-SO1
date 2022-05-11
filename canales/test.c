#include "channel.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define MENSAJES 10
#define ESCRITORES 5
#define LECTORES 5
#define DEBUG 1

struct channel chan;
pthread_mutex_t mutex_contadores = PTHREAD_MUTEX_INITIALIZER;
int envios = 0;
int recibidos = 0;

void test_uno_uno();
void test_uno_muchos();
void test_muchos_uno();
void test_muchos_muchos();

int main() {

  channel_init(&chan);

  test_uno_uno(); // un lector, un escritor
  // test_uno_muchos(); // un lector, muchos escritores
  // test_muchos_uno(); // muchos lectores, un escritor
  // test_muchos_muchos(); // muchos lectores, muchos escritores

  return 0;
}

void aumentar_envios() {
  pthread_mutex_lock(&mutex_contadores);
  envios++;
  pthread_mutex_unlock(&mutex_contadores);
}

void aumentar_recibidos() {
  pthread_mutex_lock(&mutex_contadores);
  recibidos++;
  pthread_mutex_unlock(&mutex_contadores);
}

void reset_contadores() {
  envios = 0;
  recibidos = 0;
}

void *escritor (void *arg) {
  for (int i = 0; i < MENSAJES; i++) {
    int sent = random() % 1000;
    if (DEBUG)
      printf("[ESCRITOR] Envio: %d\n", sent);
    chan_write(&chan, sent);
    if (DEBUG)
      printf("[ESCRITOR] Envie: %d\n", sent);
    aumentar_envios();
  }
}

void *lector (void *arg) {
  for (int i = 0; i < MENSAJES; i++) {
    int recvd = -1;
    if (DEBUG)
      printf("[LECTOR] Espero: %d\n", recvd);
    recvd = chan_read(&chan);
    if (DEBUG)
      printf("[LECTOR] Recibi: %d\n", recvd);
    aumentar_recibidos();
  }
}

void test_uno_uno() {
  pthread_t thread_lector;
  pthread_t thread_escritor;
  reset_contadores();

  pthread_create(&thread_lector, NULL, lector, NULL);
  pthread_create(&thread_escritor, NULL, escritor, NULL);

  pthread_join(thread_lector, NULL);
  pthread_join(thread_escritor, NULL);
  printf("[UNO UNO]\n");
  printf("ENVIOS: %d, RECIBIDOS: %d\n", envios, recibidos);
}

void test_uno_muchos() {
  // un lector, muchos escritores
  pthread_t thread_lector;
  pthread_t threads_escritor[ESCRITORES];
  reset_contadores();

  pthread_create(&thread_lector, NULL, lector, NULL);
  for (int i = 0; i < ESCRITORES; i++) {
    pthread_create(&threads_escritor[i], NULL, escritor, NULL);
  }

  pthread_join(thread_lector, NULL);
  for (int i = 0; i < ESCRITORES; i++) {
    pthread_cancel(threads_escritor[i]);
  }
  printf("[UNO MUCHOS]\n");
  printf("ENVIOS: %d, RECIBIDOS: %d\n", envios, recibidos);
}

void test_muchos_uno() {
  // muchos lectores, un escritor
  pthread_t threads_lector[LECTORES];
  pthread_t thread_escritor;
  reset_contadores();

  pthread_create(&thread_escritor, NULL, escritor, NULL);
  for (int i = 0; i < LECTORES; i++) {
    pthread_create(&threads_lector[i], NULL, lector, NULL);
  }

  pthread_join(thread_escritor, NULL);
  for (int i = 0; i < LECTORES; i++) {
    pthread_cancel(threads_lector[i]);
  }
  printf("[MUCHOS UNO]\n");
  printf("ENVIOS: %d, RECIBIDOS: %d\n", envios, recibidos);
}

void test_muchos_muchos() {
  // muchos lectores, un escritor
  pthread_t threads_lector[LECTORES];
  pthread_t threads_escritor[ESCRITORES];
  reset_contadores();

  for (int i = 0; i < ESCRITORES; i++) {
    pthread_create(&threads_escritor[i], NULL, escritor, NULL);
  }
  for (int i = 0; i < LECTORES; i++) {
    pthread_create(&threads_lector[i], NULL, lector, NULL);
  }

  for (int i = 0; i < ESCRITORES; i++) {
    pthread_join(threads_escritor[i], NULL);
  }
  for (int i = 0; i < LECTORES; i++) {
    pthread_join(threads_lector[i], NULL);
  }
  printf("[MUCHOS MUCHOS]\n");
  printf("ENVIOS: %d, RECIBIDOS: %d\n", envios, recibidos);
}
