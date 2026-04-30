#ifndef T_BUFFER_H
#define T_BUFFER_H

#include <fcntl.h>
#define TIPO char
#define TBUFFER buffer
#define N 5
#define NUM_ITERACIONS 80
#define NOM_ARQUIVO "./prod-con.tmp"
#define NOM_MQ_PROD "/ALMACENPROD"
#define NOM_MQ_CONS "/ALMACENCONS"
#include <mqueue.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

// Enumeración de conveniencia para os estados do semáforo, para a espera activa
enum estadoBuffer { BALDERIO, CHEO, INTERMEDIO };

// Tipos de datos do búfer
typedef struct buffer {
  unsigned int comezo, fin, tam;
  TIPO contidos[N];
  mqd_t almacenEntrada;
  mqd_t almacenSaida;

} buffer;

void inicializarBuffer(TBUFFER *b) {
  b->comezo = 0;
  b->fin = 0;
  b->tam = 0;

  // Poñer os atributos da cola
  struct mq_attr atribCola;
  atribCola.mq_maxmsg = N;
  atribCola.mq_msgsize = sizeof(char);
  mq_unlink(NOM_MQ_PROD);
  mq_unlink(NOM_MQ_CONS);

  // Crear a cola
  b->almacenEntrada = mq_open(NOM_MQ_PROD, O_CREAT | O_RDWR, 0777, &atribCola);
  b->almacenSaida = mq_open(NOM_MQ_CONS, O_CREAT | O_RDWR, 0777, &atribCola);
}

void librarBuffer(TBUFFER *b) {
  mq_close(b->almacenEntrada);
  mq_close(b->almacenSaida);
  mq_unlink(NOM_MQ_PROD);
  mq_unlink(NOM_MQ_CONS);
}

unsigned int seguinte(unsigned int pos) { return pos < N - 1 ? pos + 1 : 0; }

int estadoBuffer(TBUFFER b) {
  if (b.tam == N)
    return CHEO;
  else if (b.tam == 0)
    return BALDERIO;
  else
    return INTERMEDIO;
}

int inserirBuffer(TBUFFER *b, TIPO c) {
  // Se está cheo, non inserir
  if (estadoBuffer(*b) == CHEO)
    return EXIT_FAILURE;

  b->contidos[b->fin] = c;
  b->fin = seguinte(b->fin);
  sleep(1);
  b->tam++;

  return EXIT_SUCCESS;
}

int eliminarBuffer(TBUFFER *b, TIPO *c) {
  // Se está baldeiro, non eliminar
  if (estadoBuffer(*b) == BALDERIO)
    return EXIT_FAILURE;

  *c = b->contidos[b->comezo];
  b->comezo = seguinte(b->comezo);
  sleep(1);
  b->tam--;

  return EXIT_SUCCESS;
}

#endif