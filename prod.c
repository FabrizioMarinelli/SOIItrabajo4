#include "buffer.h"
#include <fcntl.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define ARQUIVO_LECTURA argv[1]
#define T atoi(argv[2])

// Cola para recibir mensajes vacíos
mqd_t almacenEntrada;
// Cola para enviar mensajes con items al consumidor
mqd_t almacenSaida;

// Funcion para extraer una letra del archivo
TIPO produce_item(FILE *arquivo) {
  char extraccion = (char)fgetc(arquivo);
  printf("Producido %c\n", extraccion);
  return extraccion;
}

void productor(FILE *arquivo, int t) {
  TIPO item = '\0';
  char vacio;
  do {

    // Producir item y esperar tiempo random
    item = produce_item(arquivo);
    usleep(rand() % (t + 1));

    // Observar si el consumidor libero un espacio
    mq_receive(almacenEntrada, &vacio, sizeof(char), NULL);

    // Enviar el item al consumidor
    mq_send(almacenSaida, &item, sizeof(TIPO), 0);

  } while (item != EOF);
}

int main(int argc, char **argv) {

  // Comprobar que hai polo menos dous argumentos
  if (argc < 3) {
    printf("Uso: %s <nome de arquivo> <valor de T>\n", argv[0]);
    return EXIT_FAILURE;
  }

  srand(time(NULL));

  // Establecer atributos de las colas de mensajes
  struct mq_attr atribCola;
  atribCola.mq_maxmsg = N;
  atribCola.mq_msgsize = sizeof(char);

  // Borrado previo por si existían de una ejecución anterior
  mq_unlink(NOM_MQ_PROD);
  mq_unlink(NOM_MQ_CONS);

  // Abrir cola de entrada
  almacenEntrada = mq_open(NOM_MQ_PROD, O_CREAT | O_RDONLY, 0777, &atribCola);

  // Abrir cola de saída
  almacenSaida = mq_open(NOM_MQ_CONS, O_CREAT | O_WRONLY, 0777, &atribCola);

  // Comprobar la creacion correcta de las colas
  if (almacenEntrada == -1 || almacenSaida == -1) {
    perror("Erro ao abrir as colas de mensaxes");
    return EXIT_FAILURE;
  }

  // Abrir o arquivo de procesamento para lectura normal
  FILE *arquivo = fopen(ARQUIVO_LECTURA, "r");

  // Comprobar la apertura correcta del archivo
  if (arquivo == NULL) {
    printf("Erro ao abrir o arquivo %s", ARQUIVO_LECTURA);
    return EXIT_FAILURE;
  }

  productor(arquivo, T);

  // Dealocar os recursos
  fclose(arquivo);
  mq_close(almacenEntrada);
  mq_close(almacenSaida);
  mq_unlink(NOM_MQ_PROD);
  mq_unlink(NOM_MQ_CONS);

  return EXIT_SUCCESS;
}