#include "buffer.h"
#include <fcntl.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

mqd_t entradaMensajesConsumidor;
mqd_t salidaMensajesProductor;

void consumirItems(FILE *archivo, char elemento) {

  fputc(elemento, archivo);
  printf("Consumido %c\n", elemento);
  return;
}

int main(int argc, char **argv) {

  // Comprobar argumentos
  if (argc < 3) {
    printf("Uso: %s <nombreArchivo> <valor de T>\n", argv[0]);
    return 1;
  }

  int t = atoi(argv[2]);
  srand(time(NULL));

  char elemento;
  char vacio = 0;

  // Obtener area de memoria del búfer compartido (creada por prod.c)
  int idArquivoCompartido = open(NOM_ARQUIVO, O_RDWR);

  if (idArquivoCompartido == -1) {
    perror("Erro ao abrir o arquivo de uso de búfer");
    return EXIT_FAILURE;
  }

  // Mapear el archivo a memoria y ligarlo con el bufer
  TBUFFER *b_compartido = mmap(NULL, sizeof(TBUFFER), PROT_READ | PROT_WRITE,
                               MAP_SHARED, idArquivoCompartido, 0);
  if (b_compartido == MAP_FAILED) {
    perror("Erro ao facer mmap");
    return EXIT_FAILURE;
  }

  // Crear cola de mensajes del consumidor
  struct mq_attr attr;
  attr.mq_maxmsg = N;
  attr.mq_msgsize = sizeof(char);
  entradaMensajesConsumidor =
      mq_open(NOM_MQ_CONS, O_CREAT | O_RDONLY, 0777, &attr);
  if (entradaMensajesConsumidor == -1) {
    perror("Error al abrir la cola de mensajes");
    return 1;
  }

  // Abrir cola de mensajes del productor (para enviar vacíos)
  salidaMensajesProductor =
      mq_open(NOM_MQ_PROD, O_CREAT | O_WRONLY, 0777, &attr);
  if (salidaMensajesProductor == -1) {
    perror("Error al abrir la cola del productor");
    return 1;
  }

  // Abrir y comprobar archivo
  FILE *f = fopen(argv[1], "w");
  if (f == NULL) {
    perror("Error al abrir el archivo");
    return 1;
  }

  // Enviar N mensajes vacíos al productor (créditos iniciales)
  for (int i = 0; i < N; i++) {
    mq_send(salidaMensajesProductor, &vacio, sizeof(char), 0);
  }

  while (1) {
    // Recibir item del productor
    mq_receive(entradaMensajesConsumidor, &elemento, sizeof(char), NULL);

    // Espera aleatoria entre 0 y T microsegundos
    usleep(rand() % (t + 1));

    // Si recibimos el fin de archivo (EOF), salimos del bucle
    if (elemento == (char)EOF) {
      break;
    }

    // Consumir el item (escribir en el archivo)
    consumirItems(f, elemento);

    // Enviar mensaje vacío de vuelta al productor (liberar crédito)
    mq_send(salidaMensajesProductor, &vacio, sizeof(char), 0);
  }

  // Cerrar colas y archivo
  fclose(f);
  mq_close(entradaMensajesConsumidor);
  mq_close(salidaMensajesProductor);

  // Liberar memoria compartida
  munmap(b_compartido, sizeof(TBUFFER));
  close(idArquivoCompartido);

  return 0;
}