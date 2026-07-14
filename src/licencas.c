#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "licencas.h"
#include "relatorios.h"
#include "utilitarios.h"

#define FICHEIRO_LICENCAS "dados/licencas.bin"

static void adicionarLicencaLista(Licenca **licencas, Licenca *novaLicenca) {
    Licenca *atual;

    novaLicenca->seguinte = NULL;

    if (*licencas == NULL) {
        *licencas = novaLicenca;
        return;
    }

    atual = *licencas;
    while (atual->seguinte != NULL) {
        atual = atual->seguinte;
    }

    atual->seguinte = novaLicenca;
}

static int obterProximoIdLicenca(Licenca *licencas) {
    int maiorId = 0;

    while (licencas != NULL) {
        if (licencas->id > maiorId) {
            maiorId = licencas->id;
        }

        licencas = licencas->seguinte;
    }

    return maiorId + 1;
}

static const char *textoTipoLicenca(TipoLicenca tipoLicenca) {
    switch (tipoLicenca) {
        case LICENCA_PERPETUA:
            return "Perpétua";
        case LICENCA_ANUAL:
            return "Subscrição anual";
        case LICENCA_OEM:
            return "OEM";
        case LICENCA_EDUCACIONAL:
            return "Educacional";
        default:
            return "Desconhecido";
    }
}

static const char *textoEstadoLicenca(EstadoLicenca estado) {
    switch (estado) {
        case ESTADO_ATIVA:
            return "Ativa";
        case ESTADO_EXPIRADA:
            return "Expirada";
        case ESTADO_SUSPENSA:
            return "Suspensa";
        case ESTADO_REVOGADA:
            return "Revogada";
        default:
            return "Desconhecido";
    }
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

static int licencaTemAtribuicoes(Atribuicao *atribuicoes, int idLicenca) {
    while (atribuicoes != NULL) {
        if (atribuicoes->idLicenca == idLicenca) {
            return 1;
        }

        atribuicoes = atribuicoes->seguinte;
    }

    return 0;
}

static int licencaTemPedidosRenovacao(PedidoRenovacao *pedidos, int idLicenca) {
    while (pedidos != NULL) {
        if (pedidos->idLicenca == idLicenca) {
            return 1;
        }

        pedidos = pedidos->seguinte;
    }

    return 0;
}

static int marcarExpiradaSeNecessario(Licenca *licenca) {
    Data dataAtual = obterDataAtual();

    /* Licenças perpétuas nunca expiram. */
    if (licenca->tipoLicenca == LICENCA_PERPETUA) {
        return 0;
    }

    if (licenca->estado == ESTADO_ATIVA
        && compararDatas(licenca->dataValidade, dataAtual) < 0) {
        licenca->estado = ESTADO_EXPIRADA;
        return 1;
    }

    return 0;
}

static void mostrarDetalhesLicenca(Licenca *licenca) {
    printf("\nID: %d\n", licenca->id);
    printf("Produto: %s\n", licenca->nomeProduto);
    printf("Fabricante: %s\n", licenca->fabricante);
    printf("Tipo: %s\n", textoTipoLicenca(licenca->tipoLicenca));
    printf("Chave de ativação: %s\n", licenca->chaveAtivacao);
    printf("Postos permitidos: %d\n", licenca->postosPermitidos);

    printf("Data de aquisição: ");
    mostrarData(licenca->dataAquisicao);
    printf("\n");

    if (licenca->tipoLicenca == LICENCA_PERPETUA) {
        printf("Data de validade: Sem validade (perpétua)\n");
    } else {
        printf("Data de validade: ");
        mostrarData(licenca->dataValidade);
        printf("\n");
    }

    printf("Custo: %.2f\n", licenca->custo);
    printf("Estado: %s\n", textoEstadoLicenca(licenca->estado));
}

static int lerInteiroEntre(const char *mensagem, int minimo, int maximo) {
    int valor;

    do {
        valor = lerInteiro(mensagem);

        if (valor < minimo || valor > maximo) {
            printf("Valor inválido. Introduza um valor entre %d e %d.\n", minimo, maximo);
        }
    } while (valor < minimo || valor > maximo);

    return valor;
}

static int lerInteiroPositivo(const char *mensagem) {
    int valor;

    do {
        valor = lerInteiro(mensagem);

        if (valor <= 0) {
            printf("O valor deve ser maior que zero.\n");
        }
    } while (valor <= 0);

    return valor;
}

static float lerFloatNaoNegativo(const char *mensagem) {
    float valor;

    do {
        valor = lerFloat(mensagem);

        if (valor < 0) {
            printf("O valor não pode ser negativo.\n");
        }
    } while (valor < 0);

    return valor;
}

static Data lerDataValidade(Data dataAquisicao) {
    Data dataValidade;

    do {
        dataValidade = lerData("Data de validade:");

        if (compararDatas(dataValidade, dataAquisicao) < 0) {
            printf("A data de validade não pode ser anterior à data de aquisição.\n");
        }
    } while (compararDatas(dataValidade, dataAquisicao) < 0);

    return dataValidade;
}

static void trocarDadosLicenca(Licenca *primeira, Licenca *segunda) {
    Licenca temporaria = *primeira;
    Licenca *seguintePrimeira = primeira->seguinte;
    Licenca *seguinteSegunda = segunda->seguinte;

    *primeira = *segunda;
    *segunda = temporaria;

    primeira->seguinte = seguintePrimeira;
    segunda->seguinte = seguinteSegunda;
}

static int deveTrocarLicencas(Licenca *primeira, Licenca *segunda, int opcaoOrdenacao) {
    switch (opcaoOrdenacao) {
        case 1:
            return strcmp(primeira->nomeProduto, segunda->nomeProduto) > 0;
        case 2:
            return compararDatas(primeira->dataAquisicao, segunda->dataAquisicao) > 0;
        case 3:
            return compararDatas(primeira->dataValidade, segunda->dataValidade) > 0;
        default:
            return 0;
    }
}

Licenca *carregarLicencas(void) {
    FILE *ficheiro;
    Licenca temporaria;
    Licenca *licencas = NULL;
    Licenca *novaLicenca;

    ficheiro = fopen(FICHEIRO_LICENCAS, "rb");

    if (ficheiro == NULL) {
        return NULL;
    }

    while (fread(&temporaria, sizeof(Licenca), 1, ficheiro) == 1) {
        novaLicenca = malloc(sizeof(Licenca));

        if (novaLicenca == NULL) {
            printf("Erro de memória ao carregar licenças.\n");
            fclose(ficheiro);
            return licencas;
        }

        *novaLicenca = temporaria;
        adicionarLicencaLista(&licencas, novaLicenca);
    }

    fclose(ficheiro);
    return licencas;
}

void guardarLicencas(Licenca *licencas) {
    FILE *ficheiro;
    Licenca *atual = licencas;
    Licenca copia;

    ficheiro = fopen(FICHEIRO_LICENCAS, "wb");

    if (ficheiro == NULL) {
        printf("Erro ao guardar licenças.\n");
        return;
    }

    while (atual != NULL) {
        /* A cópia binária não deve guardar endereços de memória. */
        copia = *atual;
        copia.seguinte = NULL;
        fwrite(&copia, sizeof(Licenca), 1, ficheiro);
        atual = atual->seguinte;
    }

    fclose(ficheiro);
}

void atualizarLicencasExpiradas(Licenca *licencas) {
    Licenca *atual = licencas;
    int houveAlteracoes = 0;

    while (atual != NULL) {
        if (marcarExpiradaSeNecessario(atual)) {
            registarAuditoria("Expirar licença automaticamente", atual->id, atual->nomeProduto);
            houveAlteracoes = 1;
        }

        atual = atual->seguinte;
    }

    if (houveAlteracoes) {
        guardarLicencas(licencas);
    }
}

void adicionarLicenca(Licenca **licencas) {
    Licenca *novaLicenca = malloc(sizeof(Licenca));

    if (novaLicenca == NULL) {
        printf("Erro ao criar licença.\n");
        return;
    }

    novaLicenca->id = obterProximoIdLicenca(*licencas);
    lerTextoObrigatorio("Nome do produto: ", novaLicenca->nomeProduto, MAX_NOME);
    lerTextoObrigatorio("Fabricante: ", novaLicenca->fabricante, MAX_NOME);

    printf("\nTipos de licença:\n");
    printf("1. Perpétua\n");
    printf("2. Subscrição anual\n");
    printf("3. OEM\n");
    printf("4. Educacional\n");
    novaLicenca->tipoLicenca = (TipoLicenca)lerInteiroEntre("Tipo: ", 1, 4);

    lerTextoObrigatorio("Chave de ativação: ", novaLicenca->chaveAtivacao, MAX_CHAVE);
    novaLicenca->postosPermitidos = lerInteiroPositivo("Número de postos permitidos: ");
    novaLicenca->dataAquisicao = lerData("Data de aquisição:");

    /* Licenças perpétuas não têm data de validade. */
    if (novaLicenca->tipoLicenca == LICENCA_PERPETUA) {
        novaLicenca->dataValidade.dia = 31;
        novaLicenca->dataValidade.mes = 12;
        novaLicenca->dataValidade.ano = 9999;
        printf("Licença perpétua: sem data de validade.\n");
    } else {
        novaLicenca->dataValidade = lerDataValidade(novaLicenca->dataAquisicao);
    }

    novaLicenca->custo = lerFloatNaoNegativo("Custo: ");

    printf("\nEstados da licença:\n");
    printf("1. Ativa\n");
    printf("2. Expirada\n");
    printf("3. Suspensa\n");
    printf("4. Revogada\n");
    novaLicenca->estado = (EstadoLicenca)lerInteiroEntre("Estado: ", 1, 4);

    if (marcarExpiradaSeNecessario(novaLicenca)) {
        printf("A licença foi marcada como expirada porque a validade já terminou.\n");
    }

    adicionarLicencaLista(licencas, novaLicenca);
    guardarLicencas(*licencas);
    registarAuditoria("Adicionar licença", novaLicenca->id, novaLicenca->nomeProduto);
    printf("Licença adicionada com sucesso. ID: %d\n", novaLicenca->id);
}

void listarLicencas(Licenca *licencas) {
    if (licencas == NULL) {
        printf("Ainda não existem licenças registadas.\n");
        return;
    }

    printf("\n=== Lista de Licenças ===\n");

    while (licencas != NULL) {
        mostrarDetalhesLicenca(licencas);
        licencas = licencas->seguinte;
    }
}

void editarLicenca(Licenca *licencas) {
    int id;
    int opcao;
    Licenca *licenca;

    if (licencas == NULL) {
        printf("Ainda não existem licenças registadas.\n");
        return;
    }

    id = lerInteiro("ID da licença a editar: ");
    licenca = procurarLicencaPorId(licencas, id);

    if (licenca == NULL) {
        printf("Licença não encontrada.\n");
        return;
    }

    do {
        printf("\n=== Editar Licença ID %d ===\n", licenca->id);
        printf("1. Nome do produto\n");
        printf("2. Fabricante\n");
        printf("3. Tipo\n");
        printf("4. Chave de ativação\n");
        printf("5. Número de postos\n");
        printf("6. Data de aquisição\n");
        printf("7. Data de validade\n");
        printf("8. Custo\n");
        printf("9. Estado\n");
        printf("0. Voltar\n");
        opcao = lerInteiro("Opção: ");

        switch (opcao) {
            case 1:
                lerTextoObrigatorio("Novo nome do produto: ", licenca->nomeProduto, MAX_NOME);
                break;
            case 2:
                lerTextoObrigatorio("Novo fabricante: ", licenca->fabricante, MAX_NOME);
                break;
            case 3:
                printf("\nTipos de licença:\n");
                printf("1. Perpétua\n");
                printf("2. Subscrição anual\n");
                printf("3. OEM\n");
                printf("4. Educacional\n");
                licenca->tipoLicenca = (TipoLicenca)lerInteiroEntre("Novo tipo: ", 1, 4);

                /* Se passou a perpétua, remove a data de validade. */
                if (licenca->tipoLicenca == LICENCA_PERPETUA) {
                    licenca->dataValidade.dia = 31;
                    licenca->dataValidade.mes = 12;
                    licenca->dataValidade.ano = 9999;
                    printf("Licença perpétua: data de validade removida.\n");
                }
                break;
            case 4:
                lerTextoObrigatorio("Nova chave de ativação: ", licenca->chaveAtivacao, MAX_CHAVE);
                break;
            case 5:
                licenca->postosPermitidos = lerInteiroPositivo("Novo número de postos: ");
                break;
            case 6:
                licenca->dataAquisicao = lerData("Nova data de aquisição:");
                if (compararDatas(licenca->dataValidade, licenca->dataAquisicao) < 0) {
                    printf("A validade atual ficou anterior à aquisição. Introduza uma nova validade.\n");
                    licenca->dataValidade = lerDataValidade(licenca->dataAquisicao);
                }
                break;
            case 7:
                if (licenca->tipoLicenca == LICENCA_PERPETUA) {
                    printf("Licenças perpétuas não têm data de validade.\n");
                } else {
                    licenca->dataValidade = lerDataValidade(licenca->dataAquisicao);
                }
                break;
            case 8:
                licenca->custo = lerFloatNaoNegativo("Novo custo: ");
                break;
            case 9:
                printf("\nEstados da licença:\n");
                printf("1. Ativa\n");
                printf("2. Expirada\n");
                printf("3. Suspensa\n");
                printf("4. Revogada\n");
                licenca->estado = (EstadoLicenca)lerInteiroEntre("Novo estado: ", 1, 4);
                break;
            case 0:
                break;
            default:
                printf("Opção inválida.\n");
        }

        if (opcao >= 1 && opcao <= 9) {
            if (marcarExpiradaSeNecessario(licenca)) {
                printf("A licença foi marcada como expirada porque a validade já terminou.\n");
            }

            guardarLicencas(licencas);
            registarAuditoria("Editar licença", licenca->id, licenca->nomeProduto);
            printf("Licença atualizada com sucesso.\n");
        }
    } while (opcao != 0);
}

void removerLicenca(Licenca **licencas, Atribuicao *atribuicoes, PedidoRenovacao *pedidos) {
    int id;
    Licenca *atual;
    Licenca *anterior = NULL;

    if (*licencas == NULL) {
        printf("Ainda não existem licenças registadas.\n");
        return;
    }

    id = lerInteiro("ID da licença a remover: ");
    atual = *licencas;

    while (atual != NULL && atual->id != id) {
        anterior = atual;
        atual = atual->seguinte;
    }

    if (atual == NULL) {
        printf("Licença não encontrada.\n");
        return;
    }

    if (licencaTemAtribuicoes(atribuicoes, id) || licencaTemPedidosRenovacao(pedidos, id)) {
        printf("Não é possível remover uma licença com atribuições ou pedidos de renovação associados.\n");
        return;
    }

    if (anterior == NULL) {
        *licencas = atual->seguinte;
    } else {
        anterior->seguinte = atual->seguinte;
    }

    registarAuditoria("Remover licença", atual->id, atual->nomeProduto);
    free(atual);
    guardarLicencas(*licencas);
    printf("Licença removida com sucesso.\n");
}

void menuOrdenarLicencas(Licenca **licencas) {
    int opcao;
    int houveTroca;
    Licenca *atual;

    if (*licencas == NULL || (*licencas)->seguinte == NULL) {
        printf("Não existem licenças suficientes para ordenar.\n");
        return;
    }

    printf("\n=== Ordenar Licenças ===\n");
    printf("1. Produto\n");
    printf("2. Data de aquisição\n");
    printf("3. Data de validade\n");
    printf("0. Voltar\n");
    opcao = lerInteiro("Opção: ");

    if (opcao == 0) {
        return;
    }

    if (opcao < 1 || opcao > 3) {
        printf("Opção inválida.\n");
        return;
    }

    do {
        houveTroca = 0;
        atual = *licencas;

        /* Trocam-se apenas os dados para preservar as ligações da lista. */
        while (atual->seguinte != NULL) {
            if (deveTrocarLicencas(atual, atual->seguinte, opcao)) {
                trocarDadosLicenca(atual, atual->seguinte);
                houveTroca = 1;
            }

            atual = atual->seguinte;
        }
    } while (houveTroca);

    guardarLicencas(*licencas);
    printf("Licenças ordenadas com sucesso.\n");
}

void menuPesquisarLicencas(Licenca *licencas) {
    int opcao;
    int id;
    int encontrou = 0;
    char textoPesquisa[MAX_NOME];
    Licenca *atual;
    Licenca *licenca;

    if (licencas == NULL) {
        printf("Ainda não existem licenças registadas.\n");
        return;
    }

    printf("\n=== Pesquisar Licenças ===\n");
    printf("1. Pesquisar por ID\n");
    printf("2. Pesquisar por produto\n");
    printf("3. Pesquisar por fabricante\n");
    printf("0. Voltar\n");
    opcao = lerInteiro("Opção: ");

    switch (opcao) {
        case 1:
            id = lerInteiro("ID da licença: ");
            licenca = procurarLicencaPorId(licencas, id);

            if (licenca == NULL) {
                printf("Licença não encontrada.\n");
            } else {
                mostrarDetalhesLicenca(licenca);
            }
            break;
        case 2:
            lerTextoObrigatorio("Nome ou parte do produto: ", textoPesquisa, MAX_NOME);
            atual = licencas;

            while (atual != NULL) {
                if (strstr(atual->nomeProduto, textoPesquisa) != NULL) {
                    encontrou = 1;
                    mostrarDetalhesLicenca(atual);
                }

                atual = atual->seguinte;
            }

            if (!encontrou) {
                printf("Não foram encontradas licenças com esse produto.\n");
            }
            break;
        case 3:
            lerTextoObrigatorio("Nome ou parte do fabricante: ", textoPesquisa, MAX_NOME);
            atual = licencas;

            while (atual != NULL) {
                if (strstr(atual->fabricante, textoPesquisa) != NULL) {
                    encontrou = 1;
                    mostrarDetalhesLicenca(atual);
                }

                atual = atual->seguinte;
            }

            if (!encontrou) {
                printf("Não foram encontradas licenças desse fabricante.\n");
            }
            break;
        case 0:
            break;
        default:
            printf("Opção inválida.\n");
    }
}

void listarLicencasPorEstado(Licenca *licencas) {
    int estado;
    int encontrou = 0;

    if (licencas == NULL) {
        printf("Ainda não existem licenças registadas.\n");
        return;
    }

    printf("\nEstados da licença:\n");
    printf("1. Ativa\n");
    printf("2. Expirada\n");
    printf("3. Suspensa\n");
    printf("4. Revogada\n");
    estado = lerInteiroEntre("Estado a listar: ", 1, 4);

    printf("\n=== Licenças por Estado ===\n");

    while (licencas != NULL) {
        if (licencas->estado == (EstadoLicenca)estado) {
            encontrou = 1;
            mostrarDetalhesLicenca(licencas);
        }

        licencas = licencas->seguinte;
    }

    if (!encontrou) {
        printf("Não existem licenças com esse estado.\n");
    }
}

void libertarLicencas(Licenca *licencas) {
    Licenca *atual = licencas;

    while (atual != NULL) {
        Licenca *seguinte = atual->seguinte;
        free(atual);
        atual = seguinte;
    }
}
