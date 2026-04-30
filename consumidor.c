#include "buffer.h"
#include <fcntl.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

mqd_t entradaMensajesConsumidor;
mqd_t salidaMensajesProductor;

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
  char vacio = 0;

  // Crear cola de mensajes del consumidor
  struct mq_attr attr;
  attr.mq_maxmsg = N;
  attr.mq_msgsize = sizeof(char);
  char dirMqCons[64];
  snprintf(dirMqCons, sizeof(dirMqCons), "/%s", NOM_MQ_CONS);

  entradaMensajesConsumidor =
      mq_open(dirMqCons, O_CREAT | O_RDONLY, 0777, &attr);
  if (entradaMensajesConsumidor == -1) {
    perror("Error al abrir la cola de mensajes");
    return 1;
  }

  // Abrir cola de mensajes del productor (para enviar vacíos)
  char dirMqProd[64];
  snprintf(dirMqProd, sizeof(dirMqProd), "/%s", NOM_MQ_PROD);
  salidaMensajesProductor = mq_open(dirMqProd, O_CREAT | O_WRONLY, 0777, &attr);
  if (salidaMensajesProductor == -1) {
    perror("Error al abrir la cola del productor");
    return 1;
  }

  // Inicializar buffer
  buffer b;
  inicializarBuffer(&b);

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

    // Consumir el item (escribir en el archivo)
    consumirItems(f, elemento);

    // Enviar mensaje vacío de vuelta al productor (liberar crédito)
    mq_send(salidaMensajesProductor, &vacio, sizeof(char), 0);
  }

  // Cerrar colas y archivo
  fclose(f);
  mq_close(entradaMensajesConsumidor);
  mq_close(salidaMensajesProductor);

  return 0;
}