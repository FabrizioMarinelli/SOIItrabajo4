#ifndef T_BUFFER_H
#define T_BUFFER_H

#include <stdlib.h>
#include <unistd.h>

#define TIPO char
#define TBUFFER buffer
#define N 5
#define NUM_ITERACIONS 80
#define NOM_ARQUIVO "/tmp/prod-con.tmp"
#define NOM_MQ_PROD "ALMACENPROD"
#define NOM_MQ_PROD "ALMACENCONS"

// Enumeración de conveniencia para os estados do semáforo, para a espera activa
enum estadoBuffer {BALDERIO, CHEO, INTERMEDIO};

// Tipos de datos do búfer
typedef struct buffer
{
	unsigned int comezo, fin, tam;
	TIPO contidos[N];
} buffer;

void inicializarBuffer(TBUFFER* b)
{
	b->comezo = 0;
	b->fin = 0;
	b->tam = 0;
}

unsigned int seguinte(unsigned int pos)
{
	return pos < N - 1 ? pos + 1 : 0;
}

int estadoBuffer(TBUFFER b)
{
	if (b.tam == N)
		return CHEO;
	else if (b.tam == 0)
		return BALDERIO;
	else
		return INTERMEDIO;
}

int inserirBuffer(TBUFFER* b, TIPO c)
{
	// Se está cheo, non inserir
	if (estadoBuffer(*b) == CHEO)
		return EXIT_FAILURE;

	b->contidos[b->fin] = c;
	b->fin = seguinte(b->fin);
	sleep(1);
	b->tam++;

	return EXIT_SUCCESS;
}

int eliminarBuffer(TBUFFER* b, TIPO* c)
{
	// Se está baldeiro, non eliminar
	if (estadoBuffer(*b) == BALDERIO)
		return EXIT_FAILURE;
	
	*c = b->contidos[b->comezo];
	b->comezo = seguinte(b->comezo);
	sleep(1);
	b->tam--;
	
	return EXIT_SUCCESS;
}

// Enumeración de conveniencia para os estados do semáforo, para a espera activa
enum estadoBuffer {BALDERIO, CHEO, INTERMEDIO};

// Tipos de datos do búfer
typedef struct buffer
{
	unsigned int comezo, fin, tam;
	TIPO contidos[N];
} buffer;

void inicializarBuffer(TBUFFER* b)
{
	b->comezo = 0;
	b->fin = 0;
	b->tam = 0;
}

unsigned int seguinte(unsigned int pos)
{
	return pos < N - 1 ? pos + 1 : 0;
}

int estadoBuffer(TBUFFER b)
{
	if (b.tam == N)
		return CHEO;
	else if (b.tam == 0)
		return BALDERIO;
	else
		return INTERMEDIO;
}

int inserirBuffer(TBUFFER* b, TIPO c)
{
	// Se está cheo, non inserir
	if (estadoBuffer(*b) == CHEO)
		return EXIT_FAILURE;

	b->contidos[b->fin] = c;
	b->fin = seguinte(b->fin);
	sleep(1);
	b->tam++;

	return EXIT_SUCCESS;
}

int eliminarBuffer(TBUFFER* b, TIPO* c)
{
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