#include "buffer.h"
#include <fcntl.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

// Cola para recibir mensajes con items del productor
mqd_t entradaMensajesConsumidor;
// Cola para enviar mensajes vacios al productor
mqd_t salidaMensajesProductor;

// Funcion para colocar una letra en el archivo
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

  // Establecer atributos de las colas de mensajes
  struct mq_attr attr;
  attr.mq_maxmsg = N;
  attr.mq_msgsize = sizeof(char);

  // Abrir cola de entrada y comprobar si la creacion fue correcta
  entradaMensajesConsumidor =
      mq_open(NOM_MQ_CONS, O_CREAT | O_RDONLY, 0777, &attr);
  if (entradaMensajesConsumidor == -1) {
    perror("Error al abrir la cola de mensajes");
    return 1;
  }

  // Abrir cola de salida y comprobar si la creacion fue correcta
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

  // Enviar N mensajes vacíos al productor (asi el productor puede trabajar)
  for (int i = 0; i < N; i++) {
    mq_send(salidaMensajesProductor, &vacio, sizeof(char), 0);
  }

  while (1) {
    // Recibir item del productor y esperar tiempo random
    mq_receive(entradaMensajesConsumidor, &elemento, sizeof(char), NULL);
    usleep(rand() % (t + 1));

    // Salir del bucle si recibimos el caracter EOF
    if (elemento == (char)EOF) {
      break;
    }

    consumirItems(f, elemento);

    // Enviar mensaje vacío de vuelta al productor
    mq_send(salidaMensajesProductor, &vacio, sizeof(char), 0);
  }

  // Cerrar colas y archivo
  fclose(f);
  mq_close(entradaMensajesConsumidor);
  mq_close(salidaMensajesProductor);

  return 0;
}