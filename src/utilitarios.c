#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include "utilitarios.h"

/* No Windows o mkdir não aceita permissões — usa-se a versão de um argumento. */
#ifdef _WIN32
#  include <direct.h>
#  define CRIAR_PASTA(p) _mkdir(p)
#else
#  define CRIAR_PASTA(p) mkdir((p), 0755)
#endif

static int criarPastaSeNecessario(const char *nomePasta) {
    if (CRIAR_PASTA(nomePasta) == 0 || errno == EEXIST) {
        return 1;
    }

    printf("Erro ao criar a pasta %s.\n", nomePasta);
    return 0;
}

int prepararPastasProjeto(void) {
    int pastaDadosCriada = criarPastaSeNecessario("dados");
    int pastaRelatoriosCriada = criarPastaSeNecessario("relatorios");

    return pastaDadosCriada && pastaRelatoriosCriada;
}

void limparBufferEntrada(void) {
    int caractere;

    while ((caractere = getchar()) != '\n' && caractere != EOF) {
    }
}

static void terminarSeFimEntrada(int resultado) {
    if (resultado == EOF) {
        printf("\nEntrada terminada. A sair do programa...\n");
        exit(0);
    }
}

int lerInteiro(const char *mensagem) {
    int valor;
    int resultado;

    do {
        printf("%s", mensagem);
        resultado = scanf("%d", &valor);
        terminarSeFimEntrada(resultado);
        limparBufferEntrada();

        if (resultado != 1) {
            printf("Valor inválido. Tente novamente.\n");
        }
    } while (resultado != 1);

    return valor;
}

float lerFloat(const char *mensagem) {
    float valor;
    int resultado;

    do {
        printf("%s", mensagem);
        resultado = scanf("%f", &valor);
        terminarSeFimEntrada(resultado);
        limparBufferEntrada();

        if (resultado != 1) {
            printf("Valor inválido. Tente novamente.\n");
        }
    } while (resultado != 1);

    return valor;
}

void lerTexto(const char *mensagem, char *destino, int tamanho) {
    size_t comprimento;

    printf("%s", mensagem);

    if (fgets(destino, tamanho, stdin) == NULL) {
        printf("\nEntrada terminada. A sair do programa...\n");
        exit(0);
    }

    comprimento = strcspn(destino, "\n");

    if (destino[comprimento] == '\n') {
        destino[comprimento] = '\0';
    } else {
        limparBufferEntrada();
    }
}

void lerTextoObrigatorio(const char *mensagem, char *destino, int tamanho) {
    do {
        lerTexto(mensagem, destino, tamanho);

        if (destino[0] == '\0') {
            printf("Este campo não pode ficar vazio.\n");
        }
    } while (destino[0] == '\0');
}

static int anoBissexto(int ano) {
    return (ano % 4 == 0 && ano % 100 != 0) || (ano % 400 == 0);
}

int dataValida(Data data) {
    int diasPorMes[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if (data.ano < 1900) {
        return 0;
    }

    if (data.mes < 1 || data.mes > 12) {
        return 0;
    }

    if (anoBissexto(data.ano)) {
        diasPorMes[1] = 29;
    }

    if (data.dia < 1 || data.dia > diasPorMes[data.mes - 1]) {
        return 0;
    }

    return 1;
}

Data lerData(const char *mensagem) {
    Data data;

    do {
        printf("%s\n", mensagem);
        data.dia = lerInteiro("Dia: ");
        data.mes = lerInteiro("Mês: ");
        data.ano = lerInteiro("Ano: ");

        if (!dataValida(data)) {
            printf("Data inválida. Tente novamente.\n");
        }
    } while (!dataValida(data));

    return data;
}

void mostrarData(Data data) {
    printf("%02d/%02d/%04d", data.dia, data.mes, data.ano);
}

int compararDatas(Data primeira, Data segunda) {
    if (primeira.ano != segunda.ano) {
        return primeira.ano - segunda.ano;
    }

    if (primeira.mes != segunda.mes) {
        return primeira.mes - segunda.mes;
    }

    return primeira.dia - segunda.dia;
}

Data obterDataAtual(void) {
    time_t instanteAtual = time(NULL);
    struct tm *dataSistema = localtime(&instanteAtual);
    Data dataAtual = {0, 0, 0};

    if (dataSistema != NULL) {
        dataAtual.dia = dataSistema->tm_mday;
        dataAtual.mes = dataSistema->tm_mon + 1;
        dataAtual.ano = dataSistema->tm_year + 1900;
    }

    return dataAtual;
}
