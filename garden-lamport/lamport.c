#include <stdlib.h>

void lock(int idProc, int nProc, int *numero, int *eligiendo)
{
  eligiendo[idProc] = 1;
  asm("mfence");
  int maximo_numero = 0;
  for (int i = 0; i < nProc; i++) {
    if (numero[i] > maximo_numero)
      maximo_numero = numero[i];
  }
  numero[idProc] = 1 + maximo_numero;
  eligiendo[idProc] = 0;

  asm("mfence");
  for (int i = 0; i < nProc; i++) {
    while (eligiendo[i]) {}

    while ((numero[i] != 0) &&
          ((numero[i] < numero[idProc]) ||
          ((numero[i] == numero[idProc]) && (i < idProc)))) {}
  }
}

void unlock(int idProc, int *numero) {
  numero[idProc] = 0;
}
