#include <stdio.h>
#include <string.h>
#include "relatorios.h"
#include "utilitarios.h"

#define FICHEIRO_RELATORIO_CUSTOS "relatorios/relatorio_custos.txt"
#define FICHEIRO_RELATORIO_UTILIZACAO "relatorios/relatorio_utilizacao.txt"
#define FICHEIRO_LICENCAS_CSV "relatorios/licencas.csv"
#define FICHEIRO_LICENCAS_TXT "relatorios/licencas.txt"
#define FICHEIRO_AUDITORIA "relatorios/auditoria.txt"

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

static int anoBissexto(int ano) {
    return (ano % 4 == 0 && ano % 100 != 0) || (ano % 400 == 0);
}

static int dataParaDias(Data data) {
    int dias = data.dia;
    int mes;
    int ano;
    int diasPorMes[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    /* Converte datas para um total comparável sem depender de bibliotecas externas. */
    for (ano = 1900; ano < data.ano; ano++) {
        dias += anoBissexto(ano) ? 366 : 365;
    }

    if (anoBissexto(data.ano)) {
        diasPorMes[1] = 29;
    }

    for (mes = 1; mes < data.mes; mes++) {
        dias += diasPorMes[mes - 1];
    }

    return dias;
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

static void escreverCampoCsv(FILE *ficheiro, const char *texto) {
    const char *caractere;
    int precisaAspas = strpbrk(texto, ",\"\n") != NULL;

    if (precisaAspas) {
        fputc('"', ficheiro);
    }

    for (caractere = texto; *caractere != '\0'; caractere++) {
        if (*caractere == '"') {
            fputc('"', ficheiro);
        }

        fputc(*caractere, ficheiro);
    }

    if (precisaAspas) {
        fputc('"', ficheiro);
    }
}

void registarAuditoria(const char *operacao, int idLicenca, const char *detalhes) {
    FILE *ficheiro = fopen(FICHEIRO_AUDITORIA, "a");

    if (ficheiro == NULL) {
        printf("Erro ao escrever no ficheiro de auditoria.\n");
        return;
    }

    fprintf(ficheiro, "Operação: %s | Licença ID: %d | Detalhes: %s\n",
            operacao,
            idLicenca,
            detalhes);

    fclose(ficheiro);
}

void mostrarHistoricoLicenca(void) {
    FILE *ficheiro;
    int idLicenca;
    int encontrou = 0;
    char linha[300];
    char padraoComDoisPontos[50];
    char padraoSemDoisPontos[50];

    idLicenca = lerInteiro("ID da licença para consultar histórico: ");
    snprintf(padraoComDoisPontos, sizeof(padraoComDoisPontos), "Licença ID: %d", idLicenca);
    snprintf(padraoSemDoisPontos, sizeof(padraoSemDoisPontos), "Licença ID %d", idLicenca);

    ficheiro = fopen(FICHEIRO_AUDITORIA, "r");

    if (ficheiro == NULL) {
        printf("Ainda não existe ficheiro de auditoria.\n");
        return;
    }

    printf("\n=== Histórico da Licença ID %d ===\n", idLicenca);

    while (fgets(linha, sizeof(linha), ficheiro) != NULL) {
        if (strstr(linha, padraoComDoisPontos) != NULL || strstr(linha, padraoSemDoisPontos) != NULL) {
            encontrou = 1;
            printf("%s", linha);
        }
    }

    fclose(ficheiro);

    if (!encontrou) {
        printf("Não existem registos de histórico para esta licença.\n");
    }
}

void gerarRelatorioCustos(Licenca *licencas) {
    FILE *ficheiro;
    int totalLicencas = 0;
    float custoTotal = 0.0f;

    ficheiro = fopen(FICHEIRO_RELATORIO_CUSTOS, "w");

    if (ficheiro == NULL) {
        printf("Erro ao criar relatório de custos.\n");
        return;
    }

    fprintf(ficheiro, "Relatório de Custos de Licenças\n");
    fprintf(ficheiro, "================================\n\n");

    while (licencas != NULL) {
        fprintf(ficheiro, "ID: %d\n", licencas->id);
        fprintf(ficheiro, "Produto: %s\n", licencas->nomeProduto);
        fprintf(ficheiro, "Fabricante: %s\n", licencas->fabricante);
        fprintf(ficheiro, "Estado: %s\n", textoEstadoLicenca(licencas->estado));
        fprintf(ficheiro, "Custo: %.2f\n\n", licencas->custo);

        totalLicencas++;
        custoTotal += licencas->custo;
        licencas = licencas->seguinte;
    }

    fprintf(ficheiro, "Total de licenças: %d\n", totalLicencas);
    fprintf(ficheiro, "Custo acumulado: %.2f\n", custoTotal);

    fclose(ficheiro);
    printf("Relatório de custos criado em %s\n", FICHEIRO_RELATORIO_CUSTOS);
}

void gerarRelatorioUtilizacao(Licenca *licencas, Atribuicao *atribuicoes) {
    FILE *ficheiro;
    int postosUtilizados;
    int postosLivres;

    ficheiro = fopen(FICHEIRO_RELATORIO_UTILIZACAO, "w");

    if (ficheiro == NULL) {
        printf("Erro ao criar relatório de utilização.\n");
        return;
    }

    fprintf(ficheiro, "Relatório de Utilização de Licenças\n");
    fprintf(ficheiro, "===================================\n\n");

    while (licencas != NULL) {
        postosUtilizados = contarAtribuicoesAtivas(atribuicoes, licencas->id);
        postosLivres = licencas->postosPermitidos - postosUtilizados;

        fprintf(ficheiro, "ID: %d\n", licencas->id);
        fprintf(ficheiro, "Produto: %s\n", licencas->nomeProduto);
        fprintf(ficheiro, "Postos permitidos: %d\n", licencas->postosPermitidos);
        fprintf(ficheiro, "Postos ocupados: %d\n", postosUtilizados);
        fprintf(ficheiro, "Postos livres: %d\n\n", postosLivres);

        licencas = licencas->seguinte;
    }

    fclose(ficheiro);
    printf("Relatório de utilização criado em %s\n", FICHEIRO_RELATORIO_UTILIZACAO);
}

void exportarLicencasCsv(Licenca *licencas) {
    FILE *ficheiro;

    ficheiro = fopen(FICHEIRO_LICENCAS_CSV, "w");

    if (ficheiro == NULL) {
        printf("Erro ao exportar licenças.\n");
        return;
    }

    fprintf(ficheiro, "ID,Produto,Fabricante,Tipo,Chave,Postos,Aquisição,Validade,Custo,Estado\n");

    while (licencas != NULL) {
        fprintf(ficheiro, "%d,", licencas->id);
        escreverCampoCsv(ficheiro, licencas->nomeProduto);
        fputc(',', ficheiro);
        escreverCampoCsv(ficheiro, licencas->fabricante);
        fputc(',', ficheiro);
        escreverCampoCsv(ficheiro, textoTipoLicenca(licencas->tipoLicenca));
        fputc(',', ficheiro);
        escreverCampoCsv(ficheiro, licencas->chaveAtivacao);
        fprintf(ficheiro, ",%d,%02d/%02d/%04d,%02d/%02d/%04d,%.2f,",
                licencas->postosPermitidos,
                licencas->dataAquisicao.dia,
                licencas->dataAquisicao.mes,
                licencas->dataAquisicao.ano,
                licencas->dataValidade.dia,
                licencas->dataValidade.mes,
                licencas->dataValidade.ano,
                licencas->custo);
        escreverCampoCsv(ficheiro, textoEstadoLicenca(licencas->estado));
        fputc('\n', ficheiro);

        licencas = licencas->seguinte;
    }

    fclose(ficheiro);
    printf("Licenças exportadas para %s\n", FICHEIRO_LICENCAS_CSV);
}

void mostrarEstatisticas(Licenca *licencas, Atribuicao *atribuicoes) {
    Licenca *inicio = licencas;
    Licenca *atual;
    Licenca *anterior;
    Licenca *outra;
    int licencasAtivas = 0;
    int postosLivresTotais = 0;
    int postosUtilizados;
    float custoTotal = 0.0f;

    if (licencas == NULL) {
        printf("Ainda não existem licenças registadas.\n");
        return;
    }

    printf("\n=== Estatísticas ===\n");

    atual = inicio;
    while (atual != NULL) {
        if (atual->estado == ESTADO_ATIVA) {
            licencasAtivas++;
        }

        postosUtilizados = contarAtribuicoesAtivas(atribuicoes, atual->id);
        postosLivresTotais += atual->postosPermitidos - postosUtilizados;
        custoTotal += atual->custo;
        atual = atual->seguinte;
    }

    atual = inicio;
    while (atual != NULL) {
        int produtoJaMostrado = 0;
        int ativasDoProduto = 0;

        for (anterior = inicio; anterior != atual; anterior = anterior->seguinte) {
            if (strcmp(anterior->nomeProduto, atual->nomeProduto) == 0) {
                produtoJaMostrado = 1;
                break;
            }
        }

        if (!produtoJaMostrado) {
            for (outra = inicio; outra != NULL; outra = outra->seguinte) {
                if (strcmp(outra->nomeProduto, atual->nomeProduto) == 0
                    && outra->estado == ESTADO_ATIVA) {
                    ativasDoProduto++;
                }
            }

            printf("Produto: %s | Licenças ativas: %d\n",
                   atual->nomeProduto,
                   ativasDoProduto);
        }

        atual = atual->seguinte;
    }

    printf("\nLicenças ativas: %d\n", licencasAtivas);
    printf("Postos livres totais: %d\n", postosLivresTotais);
    printf("Custos acumulados: %.2f\n", custoTotal);
}

void exportarLicencasTxt(Licenca *licencas) {
    FILE *ficheiro;

    ficheiro = fopen(FICHEIRO_LICENCAS_TXT, "w");

    if (ficheiro == NULL) {
        printf("Erro ao exportar licenças para TXT.\n");
        return;
    }

    fprintf(ficheiro, "Lista de Licencas de Software\n");
    fprintf(ficheiro, "=============================\n\n");

    if (licencas == NULL) {
        fprintf(ficheiro, "Nao existem licencas registadas.\n");
        fclose(ficheiro);
        printf("Lista exportada para %s\n", FICHEIRO_LICENCAS_TXT);
        return;
    }

    while (licencas != NULL) {
        fprintf(ficheiro, "ID            : %d\n", licencas->id);
        fprintf(ficheiro, "Produto       : %s\n", licencas->nomeProduto);
        fprintf(ficheiro, "Fabricante    : %s\n", licencas->fabricante);
        fprintf(ficheiro, "Tipo          : %s\n", textoTipoLicenca(licencas->tipoLicenca));
        fprintf(ficheiro, "Chave         : %s\n", licencas->chaveAtivacao);
        fprintf(ficheiro, "Postos        : %d\n", licencas->postosPermitidos);
        fprintf(ficheiro, "Aquisicao     : %02d/%02d/%04d\n",
                licencas->dataAquisicao.dia,
                licencas->dataAquisicao.mes,
                licencas->dataAquisicao.ano);

        if (licencas->tipoLicenca == LICENCA_PERPETUA) {
            fprintf(ficheiro, "Validade      : Sem validade (perpetua)\n");
        } else {
            fprintf(ficheiro, "Validade      : %02d/%02d/%04d\n",
                    licencas->dataValidade.dia,
                    licencas->dataValidade.mes,
                    licencas->dataValidade.ano);
        }

        fprintf(ficheiro, "Custo         : %.2f EUR\n", licencas->custo);
        fprintf(ficheiro, "Estado        : %s\n", textoEstadoLicenca(licencas->estado));
        fprintf(ficheiro, "-----------------------------\n");

        licencas = licencas->seguinte;
    }

    fclose(ficheiro);
    printf("Licencas exportadas para %s\n", FICHEIRO_LICENCAS_TXT);
}

void mostrarAlertasValidade(Licenca *licencas) {
    Data dataAtual;
    int diasDataAtual;
    int diasValidade;
    int diasRestantes;
    int encontrou = 0;

    if (licencas == NULL) {
        printf("Ainda não existem licenças registadas.\n");
        return;
    }

    dataAtual = obterDataAtual();
    diasDataAtual = dataParaDias(dataAtual);

    printf("\n=== Alertas de Expiração nos Próximos 30 Dias ===\n");
    printf("Data atual: ");
    mostrarData(dataAtual);
    printf("\n");

    while (licencas != NULL) {
        /* Licenças perpétuas nunca expiram — ignorar nos alertas. */
        if (licencas->tipoLicenca == LICENCA_PERPETUA) {
            licencas = licencas->seguinte;
            continue;
        }

        diasValidade = dataParaDias(licencas->dataValidade);
        diasRestantes = diasValidade - diasDataAtual;

        if (diasRestantes >= 0 && diasRestantes <= 30) {
            encontrou = 1;
            printf("ID %d | %s | Expira em %d dias | ",
                   licencas->id,
                   licencas->nomeProduto,
                   diasRestantes);
            mostrarData(licencas->dataValidade);
            printf("\n");
        }

        licencas = licencas->seguinte;
    }

    if (!encontrou) {
        printf("Não existem licenças a expirar nos próximos 30 dias.\n");
    }
}
