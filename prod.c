#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include "buffer.h"

#define ARQUIVO_LECTURA argv[1]
#define T atoi(argv[2])

TIPO produce_item(FILE* arquivo)
{
    return (char) fgetc(arquivo);
}

void productor(FILE* arquivo, int t)
{
    // Poñer os atributos da cola
    struct mq_attr atribCola;
    atribCola.mq_maxmsg = N;
    atribCola.mq_msgsize = sizeof(char);
    mq_unlink(NOM_MQ_PROD);
    mq_unlink(NOM_MQ_CONS);

    // Crear a cola
    mqd_t almacenEntrada = mq_open(NOM_MQ_PROD, O_CREAT | O_WRONLY, 0777, &atribCola);
    mqd_t almacenSaida = mq_open(NOM_MQ_CONS, O_CREAT | O_WRONLY, 0777, &atribCola);
    if (almacenEntrada == -1 || almacenSaida == -1)
    {
        printf("Erro ao abir a cola de mesaxes: Entrada: %d - Saída %d\n", almacenEntrada, almacenSaida);
        return;
    }

    TIPO item = '\0';
    do
    {
        item = produce_item(arquivo);
        usleep(rand() % (t + 1));
        mq_receive(almacenEntrada, NULL, sizeof(char), 0);
        mq_send(almacenSaida, &item, sizeof(TIPO), 0);
        
    } while (item != EOF);

    mq_close(almacenEntrada);
    mq_close(almacenSaida);
}

int main(int argc, char** argv)
{
    // Comprobar que hai polo menos dous argumentos
    if (argc < 3)
    {
        printf("Uso: %s <nome de arquivo> <valor de T>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Abrir o arquivo de uso compartido
    int idArquivoCompartido = open(NOM_ARQUIVO, O_RDWR | O_CREAT, 0777);
    if (idArquivoCompartido == -1)
    {
        printf("Erro ao abrir o arquivo de uso de búfer\n");
        return EXIT_FAILURE;
    }

    // Poñer o tamaño do búfer
    struct stat arquivoCompartido;
    ftruncate(idArquivoCompartido, sizeof(buffer));

    if (fstat(idArquivoCompartido, &arquivoCompartido) == -1)
    {
        printf("Erro ao obter os atributos do arquivo de uso de búfer\n");
        return EXIT_FAILURE;
    }

    // Proxectar o arquivo a memoria e ligalo co búfer
    TBUFFER* buffer = mmap(NULL, sizeof(buffer), PROT_READ | PROT_WRITE, MAP_SHARED, idArquivoCompartido, 0);
    if (buffer == NULL)
    {
        printf("Erro ao facer nmap\n");
        return EXIT_FAILURE;
    }

    // Abrir o arquivo de procesamento para lectura normal
    FILE* arquivo = fopen(ARQUIVO_LECTURA, "r");

    if (arquivo == NULL)
    {
        printf("Erro ao abrir o arquivo %s", ARQUIVO_LECTURA);
        munmap(buffer, arquivoCompartido.st_size);
        return EXIT_FAILURE;
    }
    
    productor(arquivo, T);

    // Dealocar os recursos
     if(close(idArquivoCompartido) == -1) 
        perror("Erro ao pechar o arquivo de lectura\n");

    if(munmap(buffer, arquivoCompartido.st_size) == -1)
        perror("Erro ao desalocar as memorias dos descriptores de arquivos\n");

    return EXIT_SUCCESS;
}
