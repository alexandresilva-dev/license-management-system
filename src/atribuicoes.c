#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "atribuicoes.h"
#include "relatorios.h"
#include "utilitarios.h"

#define FICHEIRO_ATRIBUICOES "dados/atribuicoes.bin"

static void adicionarAtribuicaoLista(Atribuicao **atribuicoes, Atribuicao *novaAtribuicao) {
    Atribuicao *atual;

    novaAtribuicao->seguinte = NULL;

    if (*atribuicoes == NULL) {
        *atribuicoes = novaAtribuicao;
        return;
    }

    atual = *atribuicoes;
    while (atual->seguinte != NULL) {
        atual = atual->seguinte;
    }

    atual->seguinte = novaAtribuicao;
}

static int obterProximoIdAtribuicao(Atribuicao *atribuicoes) {
    int maiorId = 0;

    while (atribuicoes != NULL) {
        if (atribuicoes->id > maiorId) {
            maiorId = atribuicoes->id;
        }

        atribuicoes = atribuicoes->seguinte;
    }

    return maiorId + 1;
}

static Licenca *procurarLicencaPorId(Licenca *licencas, int id) {
    while (licencas != NULL) {
        if (licencas->id == id) {
            return licencas;
        }

        licencas = licencas->seguinte;
    }

    return NULL;
}

static int contarAtribuicoesAtivas(Atribuicao *atribuicoes, int idLicenca) {
    int total = 0;

    while (atribuicoes != NULL) {
        if (atribuicoes->idLicenca == idLicenca && atribuicoes->ativa) {
            total++;
        }

        atribuicoes = atribuicoes->seguinte;
    }

    return total;
}

Atribuicao *carregarAtribuicoes(void) {
    FILE *ficheiro;
    Atribuicao temporaria;
    Atribuicao *atribuicoes = NULL;
    Atribuicao *novaAtribuicao;

    ficheiro = fopen(FICHEIRO_ATRIBUICOES, "rb");

    if (ficheiro == NULL) {
        return NULL;
    }

    while (fread(&temporaria, sizeof(Atribuicao), 1, ficheiro) == 1) {
        novaAtribuicao = malloc(sizeof(Atribuicao));

        if (novaAtribuicao == NULL) {
            printf("Erro de memória ao carregar atribuições.\n");
            fclose(ficheiro);
            return atribuicoes;
        }

        *novaAtribuicao = temporaria;
        adicionarAtribuicaoLista(&atribuicoes, novaAtribuicao);
    }

    fclose(ficheiro);
    return atribuicoes;
}

void guardarAtribuicoes(Atribuicao *atribuicoes) {
    FILE *ficheiro;
    Atribuicao *atual = atribuicoes;
    Atribuicao copia;

    ficheiro = fopen(FICHEIRO_ATRIBUICOES, "wb");

    if (ficheiro == NULL) {
        printf("Erro ao guardar atribuições.\n");
        return;
    }

    while (atual != NULL) {
        /* A lista é reconstruída quando o ficheiro volta a ser carregado. */
        copia = *atual;
        copia.seguinte = NULL;
        fwrite(&copia, sizeof(Atribuicao), 1, ficheiro);
        atual = atual->seguinte;
    }

    fclose(ficheiro);
}

void registarInstalacao(Atribuicao **atribuicoes, Licenca *licencas, const char *responsavelFixo) {
    int idLicenca;
    int postosUtilizados;
    Licenca *licenca;
    Atribuicao *novaAtribuicao;

    if (licencas == NULL) {
        printf("Ainda não existem licenças registadas.\n");
        return;
    }

    idLicenca = lerInteiro("ID da licença: ");
    licenca = procurarLicencaPorId(licencas, idLicenca);

    if (licenca == NULL) {
        printf("Licença não encontrada.\n");
        return;
    }

    if (licenca->estado != ESTADO_ATIVA) {
        printf("Só é possível instalar licenças ativas.\n");
        return;
    }

    postosUtilizados = contarAtribuicoesAtivas(*atribuicoes, idLicenca);
    if (postosUtilizados >= licenca->postosPermitidos) {
        printf("Não existem postos livres para esta licença.\n");
        return;
    }

    novaAtribuicao = malloc(sizeof(Atribuicao));

    if (novaAtribuicao == NULL) {
        printf("Erro ao criar atribuição.\n");
        return;
    }

    novaAtribuicao->id = obterProximoIdAtribuicao(*atribuicoes);
    novaAtribuicao->idLicenca = idLicenca;
    lerTextoObrigatorio("Identificador do posto/máquina: ", novaAtribuicao->identificadorPosto, MAX_NOME);

    if (responsavelFixo == NULL) {
        lerTextoObrigatorio("Responsável: ", novaAtribuicao->responsavel, MAX_NOME);
    } else {
        snprintf(novaAtribuicao->responsavel, MAX_NOME, "%s", responsavelFixo);
    }

    do {
        novaAtribuicao->dataInstalacao = lerData("Data de instalação:");

        if (compararDatas(novaAtribuicao->dataInstalacao, licenca->dataAquisicao) < 0
            || compararDatas(novaAtribuicao->dataInstalacao, licenca->dataValidade) > 0) {
            printf("A instalação deve ficar entre a data de aquisição e a data de validade.\n");
        }
    } while (compararDatas(novaAtribuicao->dataInstalacao, licenca->dataAquisicao) < 0
             || compararDatas(novaAtribuicao->dataInstalacao, licenca->dataValidade) > 0);

    novaAtribuicao->dataDesinstalacao.dia = 0;
    novaAtribuicao->dataDesinstalacao.mes = 0;
    novaAtribuicao->dataDesinstalacao.ano = 0;
    novaAtribuicao->ativa = 1;

    adicionarAtribuicaoLista(atribuicoes, novaAtribuicao);
    guardarAtribuicoes(*atribuicoes);
    registarAuditoria("Registar instalação", idLicenca, novaAtribuicao->identificadorPosto);
    printf("Instalação registada com sucesso. ID: %d\n", novaAtribuicao->id);
}

void registarDesinstalacao(Atribuicao *atribuicoes, const char *responsavelPermitido) {
    int id;
    Atribuicao *atual = atribuicoes;

    if (atribuicoes == NULL) {
        printf("Ainda não existem atribuições registadas.\n");
        return;
    }

    id = lerInteiro("ID da atribuição a desinstalar: ");

    while (atual != NULL && atual->id != id) {
        atual = atual->seguinte;
    }

    if (atual == NULL) {
        printf("Atribuição não encontrada.\n");
        return;
    }

    if (!atual->ativa) {
        printf("Esta atribuição já se encontra desinstalada.\n");
        return;
    }

    if (responsavelPermitido != NULL && strcmp(atual->responsavel, responsavelPermitido) != 0) {
        printf("Não tem permissão para desinstalar esta atribuição.\n");
        return;
    }

    do {
        atual->dataDesinstalacao = lerData("Data de desinstalação:");

        if (compararDatas(atual->dataDesinstalacao, atual->dataInstalacao) < 0) {
            printf("A data de desinstalação não pode ser anterior à data de instalação.\n");
        }
    } while (compararDatas(atual->dataDesinstalacao, atual->dataInstalacao) < 0);

    atual->ativa = 0;
    guardarAtribuicoes(atribuicoes);
    registarAuditoria("Registar desinstalação", atual->idLicenca, atual->identificadorPosto);
    printf("Desinstalação registada com sucesso.\n");
}

void listarAtribuicoes(Atribuicao *atribuicoes) {
    if (atribuicoes == NULL) {
        printf("Ainda não existem atribuições registadas.\n");
        return;
    }

    printf("\n=== Atribuições Registadas ===\n");

    while (atribuicoes != NULL) {
        printf("\nID: %d\n", atribuicoes->id);
        printf("ID da licença: %d\n", atribuicoes->idLicenca);
        printf("Posto/Máquina: %s\n", atribuicoes->identificadorPosto);
        printf("Responsável: %s\n", atribuicoes->responsavel);

        printf("Data de instalação: ");
        mostrarData(atribuicoes->dataInstalacao);
        printf("\n");

        if (atribuicoes->ativa) {
            printf("Estado: Instalada\n");
        } else {
            printf("Data de desinstalação: ");
            mostrarData(atribuicoes->dataDesinstalacao);
            printf("\n");
            printf("Estado: Desinstalada\n");
        }

        atribuicoes = atribuicoes->seguinte;
    }
}

void listarAtribuicoesPorResponsavel(Atribuicao *atribuicoes, Licenca *licencas, const char *responsavel) {
    Licenca *licenca;
    int encontrou = 0;

    if (atribuicoes == NULL) {
        printf("Ainda não existem atribuições registadas.\n");
        return;
    }

    printf("\n=== Licenças Atribuídas a %s ===\n", responsavel);

    while (atribuicoes != NULL) {
        if (strcmp(atribuicoes->responsavel, responsavel) == 0) {
            encontrou = 1;
            licenca = procurarLicencaPorId(licencas, atribuicoes->idLicenca);

            printf("\nID da atribuição: %d\n", atribuicoes->id);
            printf("Posto/Máquina: %s\n", atribuicoes->identificadorPosto);
            printf("Estado da instalação: %s\n", atribuicoes->ativa ? "Instalada" : "Desinstalada");

            if (licenca != NULL) {
                printf("Licença ID: %d\n", licenca->id);
                printf("Produto: %s\n", licenca->nomeProduto);
                printf("Fabricante: %s\n", licenca->fabricante);
                printf("Validade: ");
                mostrarData(licenca->dataValidade);
                printf("\n");
            } else {
                printf("Licença associada não encontrada.\n");
            }
        }

        atribuicoes = atribuicoes->seguinte;
    }

    if (!encontrou) {
        printf("Não existem licenças atribuídas a este responsável.\n");
    }
}

void libertarAtribuicoes(Atribuicao *atribuicoes) {
    Atribuicao *atual = atribuicoes;

    while (atual != NULL) {
        Atribuicao *seguinte = atual->seguinte;
        free(atual);
        atual = seguinte;
    }
}
