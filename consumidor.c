#include "buffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

void consumirItems(FILE *archivo, char elemento) {

  fputc(elemento, archivo);
  return;
}

int main(int argc, char **argv) {

  // Comprobar argumentos
  if (argc != 2) {
    printf("Uso: %s <nombreArchivo>\n", argv[0]);
    return 1;
  }

  char elemento;

  // Inicializar buffer
  buffer b;
  inicializarBuffer(&b);

  // Abrir y comprobar archivo
  FILE *f = fopen(argv[1], "rw");
  if (f == NULL) {
    perror("Error al abrir el archivo");
    return 1;
  }

  return 0;
}