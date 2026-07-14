#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "renovacoes.h"
#include "licencas.h"
#include "utilitarios.h"

#define FICHEIRO_RENOVACOES "dados/renovacoes.bin"
#define FICHEIRO_AUDITORIA "relatorios/auditoria.txt"

static void adicionarPedidoLista(PedidoRenovacao **pedidos, PedidoRenovacao *novoPedido) {
    PedidoRenovacao *atual;

    novoPedido->seguinte = NULL;

    if (*pedidos == NULL) {
        *pedidos = novoPedido;
        return;
    }

    atual = *pedidos;
    while (atual->seguinte != NULL) {
        atual = atual->seguinte;
    }

    atual->seguinte = novoPedido;
}

static int obterProximoIdPedido(PedidoRenovacao *pedidos) {
    int maiorId = 0;

    while (pedidos != NULL) {
        if (pedidos->id > maiorId) {
            maiorId = pedidos->id;
        }

        pedidos = pedidos->seguinte;
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

static int existePedidoPendente(PedidoRenovacao *pedidos, int idLicenca) {
    while (pedidos != NULL) {
        if (pedidos->idLicenca == idLicenca && pedidos->estado == RENOVACAO_PENDENTE) {
            return 1;
        }

        pedidos = pedidos->seguinte;
    }

    return 0;
}

static const char *textoEstadoRenovacao(EstadoRenovacao estado) {
    switch (estado) {
        case RENOVACAO_PENDENTE:
            return "Pendente";
        case RENOVACAO_APROVADA:
            return "Aprovada";
        case RENOVACAO_REJEITADA:
            return "Rejeitada";
        default:
            return "Desconhecido";
    }
}

static void registarAuditoriaRenovacao(const char *operacao, PedidoRenovacao *pedido) {
    FILE *ficheiro = fopen(FICHEIRO_AUDITORIA, "a");

    if (ficheiro == NULL) {
        printf("Erro ao escrever no ficheiro de auditoria.\n");
        return;
    }

    fprintf(ficheiro,
            "Pedido renovação ID %d | Licença ID %d | Utilizador %s | Operação: %s | Estado: %s\n",
            pedido->id,
            pedido->idLicenca,
            pedido->pedidoPor,
            operacao,
            textoEstadoRenovacao(pedido->estado));

    fclose(ficheiro);
}

PedidoRenovacao *carregarPedidosRenovacao(void) {
    FILE *ficheiro;
    PedidoRenovacao temporario;
    PedidoRenovacao *pedidos = NULL;
    PedidoRenovacao *novoPedido;

    ficheiro = fopen(FICHEIRO_RENOVACOES, "rb");

    if (ficheiro == NULL) {
        return NULL;
    }

    while (fread(&temporario, sizeof(PedidoRenovacao), 1, ficheiro) == 1) {
        novoPedido = malloc(sizeof(PedidoRenovacao));

        if (novoPedido == NULL) {
            printf("Erro de memória ao carregar pedidos de renovação.\n");
            fclose(ficheiro);
            return pedidos;
        }

        *novoPedido = temporario;
        adicionarPedidoLista(&pedidos, novoPedido);
    }

    fclose(ficheiro);
    return pedidos;
}

void guardarPedidosRenovacao(PedidoRenovacao *pedidos) {
    FILE *ficheiro;
    PedidoRenovacao *atual = pedidos;
    PedidoRenovacao copia;

    ficheiro = fopen(FICHEIRO_RENOVACOES, "wb");

    if (ficheiro == NULL) {
        printf("Erro ao guardar pedidos de renovação.\n");
        return;
    }

    while (atual != NULL) {
        /* Não se grava o apontador da lista no ficheiro binário. */
        copia = *atual;
        copia.seguinte = NULL;
        fwrite(&copia, sizeof(PedidoRenovacao), 1, ficheiro);
        atual = atual->seguinte;
    }

    fclose(ficheiro);
}

void solicitarRenovacao(PedidoRenovacao **pedidos, Licenca *licencas, const char *nomeUtilizador) {
    int idLicenca;
    Licenca *licenca;
    PedidoRenovacao *novoPedido;

    if (licencas == NULL) {
        printf("Ainda não existem licenças registadas.\n");
        return;
    }

    idLicenca = lerInteiro("ID da licença a renovar: ");
    licenca = procurarLicencaPorId(licencas, idLicenca);

    if (licenca == NULL) {
        printf("Licença não encontrada.\n");
        return;
    }

    if (existePedidoPendente(*pedidos, idLicenca)) {
        printf("Já existe um pedido de renovação pendente para esta licença.\n");
        return;
    }

    novoPedido = malloc(sizeof(PedidoRenovacao));

    if (novoPedido == NULL) {
        printf("Erro ao criar pedido de renovação.\n");
        return;
    }

    novoPedido->id = obterProximoIdPedido(*pedidos);
    novoPedido->idLicenca = idLicenca;
    snprintf(novoPedido->pedidoPor, MAX_NOME_UTILIZADOR, "%s", nomeUtilizador);
    novoPedido->dataPedido = lerData("Data do pedido:");
    novoPedido->estado = RENOVACAO_PENDENTE;

    adicionarPedidoLista(pedidos, novoPedido);
    guardarPedidosRenovacao(*pedidos);
    registarAuditoriaRenovacao("Pedido criado", novoPedido);
    printf("Pedido de renovação registado com sucesso. ID: %d\n", novoPedido->id);
}

void listarPedidosRenovacao(PedidoRenovacao *pedidos) {
    if (pedidos == NULL) {
        printf("Ainda não existem pedidos de renovação.\n");
        return;
    }

    printf("\n=== Pedidos de Renovação ===\n");

    while (pedidos != NULL) {
        printf("\nID: %d\n", pedidos->id);
        printf("ID da licença: %d\n", pedidos->idLicenca);
        printf("Pedido por: %s\n", pedidos->pedidoPor);
        printf("Data do pedido: ");
        mostrarData(pedidos->dataPedido);
        printf("\nEstado: %s\n", textoEstadoRenovacao(pedidos->estado));

        pedidos = pedidos->seguinte;
    }
}

void listarPedidosRenovacaoPorUtilizador(PedidoRenovacao *pedidos, const char *nomeUtilizador) {
    int encontrou = 0;

    printf("\n=== Os Meus Pedidos de Renovação ===\n");

    while (pedidos != NULL) {
        if (strcmp(pedidos->pedidoPor, nomeUtilizador) == 0) {
            encontrou = 1;
            printf("\nID: %d\n", pedidos->id);
            printf("ID da licença: %d\n", pedidos->idLicenca);
            printf("Data do pedido: ");
            mostrarData(pedidos->dataPedido);
            printf("\nEstado: %s\n", textoEstadoRenovacao(pedidos->estado));
        }

        pedidos = pedidos->seguinte;
    }

    if (!encontrou) {
        printf("Não existem pedidos de renovação para este utilizador.\n");
    }
}

void gerirPedidosRenovacao(PedidoRenovacao *pedidos, Licenca *licencas) {
    int id;
    int opcao;
    PedidoRenovacao *atual = pedidos;
    Licenca *licenca;

    if (pedidos == NULL) {
        printf("Ainda não existem pedidos de renovação.\n");
        return;
    }

    listarPedidosRenovacao(pedidos);
    id = lerInteiro("ID do pedido a analisar: ");

    while (atual != NULL && atual->id != id) {
        atual = atual->seguinte;
    }

    if (atual == NULL) {
        printf("Pedido não encontrado.\n");
        return;
    }

    if (atual->estado != RENOVACAO_PENDENTE) {
        printf("Este pedido já foi analisado.\n");
        return;
    }

    licenca = procurarLicencaPorId(licencas, atual->idLicenca);

    if (licenca == NULL) {
        printf("Licença associada ao pedido não foi encontrada.\n");
        return;
    }

    printf("1. Aprovar pedido\n");
    printf("2. Rejeitar pedido\n");
    printf("0. Voltar\n");
    opcao = lerInteiro("Opção: ");

    switch (opcao) {
        case 1: {
            Data validadeAtual = licenca->dataValidade;
            Data novaDataValidade;

            do {
                novaDataValidade = lerData("Nova data de validade:");

                if (compararDatas(novaDataValidade, validadeAtual) <= 0) {
                    printf("A nova data de validade deve ser posterior à validade atual.\n");
                }
            } while (compararDatas(novaDataValidade, validadeAtual) <= 0);

            licenca->dataValidade = novaDataValidade;
            licenca->estado = ESTADO_ATIVA;
            atual->estado = RENOVACAO_APROVADA;
            guardarLicencas(licencas);
            guardarPedidosRenovacao(pedidos);
            registarAuditoriaRenovacao("Pedido aprovado", atual);
            printf("Pedido aprovado e licença renovada.\n");
            break;
        }
        case 2:
            atual->estado = RENOVACAO_REJEITADA;
            guardarPedidosRenovacao(pedidos);
            registarAuditoriaRenovacao("Pedido rejeitado", atual);
            printf("Pedido rejeitado.\n");
            break;
        case 0:
            break;
        default:
            printf("Opção inválida.\n");
    }
}

void libertarPedidosRenovacao(PedidoRenovacao *pedidos) {
    PedidoRenovacao *atual = pedidos;

    while (atual != NULL) {
        PedidoRenovacao *seguinte = atual->seguinte;
        free(atual);
        atual = seguinte;
    }
}
