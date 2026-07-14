#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utilizadores.h"
#include "utilitarios.h"
#include "relatorios.h"

#define FICHEIRO_UTILIZADORES "dados/utilizadores.bin"

static void adicionarUtilizadorLista(Utilizador **utilizadores, Utilizador *novoUtilizador) {
    Utilizador *atual;

    novoUtilizador->seguinte = NULL;

    if (*utilizadores == NULL) {
        *utilizadores = novoUtilizador;
        return;
    }

    atual = *utilizadores;
    while (atual->seguinte != NULL) {
        atual = atual->seguinte;
    }

    atual->seguinte = novoUtilizador;
}

static Utilizador *criarGestorPendente(const char *nomeUtilizador) {
    Utilizador *novoUtilizador = malloc(sizeof(Utilizador));

    if (novoUtilizador == NULL) {
        printf("Erro ao criar utilizador.\n");
        return NULL;
    }

    strcpy(novoUtilizador->nomeUtilizador, nomeUtilizador);
    lerTextoObrigatorio("Palavra-passe: ", novoUtilizador->palavraPasse, MAX_PALAVRA_PASSE);
    novoUtilizador->tipoUtilizador = TIPO_GESTOR_SOFTWARE;
    novoUtilizador->validado = 0;
    novoUtilizador->primeiroAcesso = 0;
    novoUtilizador->seguinte = NULL;

    return novoUtilizador;
}

static void alterarPalavraPasseInicial(Utilizador *utilizadores, Utilizador *utilizador) {
    if (utilizador->primeiroAcesso != 1) {
        return;
    }

    printf("\nTem de alterar a palavra-passe no primeiro acesso.\n");
    lerTextoObrigatorio("Nova palavra-passe: ", utilizador->palavraPasse, MAX_PALAVRA_PASSE);
    utilizador->primeiroAcesso = 0;
    guardarUtilizadores(utilizadores);
    registarAuditoria("Alterar palavra-passe inicial", -1, utilizador->nomeUtilizador);
    printf("Palavra-passe alterada com sucesso.\n");
}

static Utilizador *validarCredenciais(Utilizador *utilizadores, Utilizador *utilizador, const char *palavraPasse) {
    if (strcmp(utilizador->palavraPasse, palavraPasse) != 0) {
        printf("Palavra-passe incorreta.\n");
        return NULL;
    }

    if (utilizador->validado == 0) {
        printf("Utilizador ainda não validado pelo administrador.\n");
        return NULL;
    }

    alterarPalavraPasseInicial(utilizadores, utilizador);
    return utilizador;
}

Utilizador *carregarUtilizadores(void) {
    FILE *ficheiro;
    Utilizador temporario;
    Utilizador *utilizadores = NULL;
    Utilizador *novoUtilizador;

    ficheiro = fopen(FICHEIRO_UTILIZADORES, "rb");

    if (ficheiro == NULL) {
        return NULL;
    }

    while (fread(&temporario, sizeof(Utilizador), 1, ficheiro) == 1) {
        novoUtilizador = malloc(sizeof(Utilizador));

        if (novoUtilizador == NULL) {
            printf("Erro de memória ao carregar utilizadores.\n");
            fclose(ficheiro);
            return utilizadores;
        }

        *novoUtilizador = temporario;
        adicionarUtilizadorLista(&utilizadores, novoUtilizador);
    }

    fclose(ficheiro);
    return utilizadores;
}

void guardarUtilizadores(Utilizador *utilizadores) {
    FILE *ficheiro;
    Utilizador *atual = utilizadores;
    Utilizador copia;

    ficheiro = fopen(FICHEIRO_UTILIZADORES, "wb");

    if (ficheiro == NULL) {
        printf("Erro ao guardar utilizadores.\n");
        return;
    }

    while (atual != NULL) {
        /* O apontador seguinte só é válido durante esta execução. */
        copia = *atual;
        copia.seguinte = NULL;
        fwrite(&copia, sizeof(Utilizador), 1, ficheiro);
        atual = atual->seguinte;
    }

    fclose(ficheiro);
}

Utilizador *procurarUtilizador(Utilizador *utilizadores, const char *nomeUtilizador) {
    Utilizador *atual = utilizadores;

    while (atual != NULL) {
        if (strcmp(atual->nomeUtilizador, nomeUtilizador) == 0) {
            return atual;
        }

        atual = atual->seguinte;
    }

    return NULL;
}

void garantirAdministradorInicial(Utilizador **utilizadores) {
    Utilizador *administrador;

    if (*utilizadores != NULL) {
        return;
    }

    administrador = malloc(sizeof(Utilizador));

    if (administrador == NULL) {
        printf("Erro ao criar administrador inicial.\n");
        return;
    }

    strcpy(administrador->nomeUtilizador, "admin");
    strcpy(administrador->palavraPasse, "admin");
    administrador->tipoUtilizador = TIPO_ADMINISTRADOR;
    administrador->validado = 1;
    administrador->primeiroAcesso = 1;

    adicionarUtilizadorLista(utilizadores, administrador);
    guardarUtilizadores(*utilizadores);
}

Utilizador *iniciarSessaoOuRegistar(Utilizador **utilizadores) {
    char nomeUtilizador[MAX_NOME_UTILIZADOR];
    char palavraPasse[MAX_PALAVRA_PASSE];
    char resposta;
    Utilizador *utilizador;
    Utilizador *novoUtilizador;

    lerTextoObrigatorio("Utilizador: ", nomeUtilizador, MAX_NOME_UTILIZADOR);
    utilizador = procurarUtilizador(*utilizadores, nomeUtilizador);

    if (utilizador == NULL) {
        printf("Utilizador não encontrado. Pretende registar-se? (s/n): ");
        scanf(" %c", &resposta);
        limparBufferEntrada();

        if (resposta != 's' && resposta != 'S') {
            return NULL;
        }

        novoUtilizador = criarGestorPendente(nomeUtilizador);

        if (novoUtilizador == NULL) {
            return NULL;
        }

        adicionarUtilizadorLista(utilizadores, novoUtilizador);
        guardarUtilizadores(*utilizadores);
        registarAuditoria("Registar gestor de software", -1, novoUtilizador->nomeUtilizador);
        printf("Gestor de Software registado. Aguarda validação do administrador.\n");
        return NULL;
    }

    lerTextoObrigatorio("Palavra-passe: ", palavraPasse, MAX_PALAVRA_PASSE);
    return validarCredenciais(*utilizadores, utilizador, palavraPasse);
}

void registarGestorSoftware(Utilizador **utilizadores) {
    char nomeUtilizador[MAX_NOME_UTILIZADOR];
    Utilizador *novoUtilizador;

    lerTextoObrigatorio("Novo utilizador: ", nomeUtilizador, MAX_NOME_UTILIZADOR);

    if (procurarUtilizador(*utilizadores, nomeUtilizador) != NULL) {
        printf("Esse nome de utilizador já existe.\n");
        return;
    }

    novoUtilizador = criarGestorPendente(nomeUtilizador);

    if (novoUtilizador == NULL) {
        return;
    }

    adicionarUtilizadorLista(utilizadores, novoUtilizador);
    guardarUtilizadores(*utilizadores);
    registarAuditoria("Registar gestor de software", -1, novoUtilizador->nomeUtilizador);
    printf("Gestor de Software registado. Aguarda validação do administrador.\n");
}

void validarGestoresSoftware(Utilizador *utilizadores) {
    Utilizador *atual = utilizadores;
    int encontrou = 0;
    char resposta;

    printf("\n=== Gestores de Software Pendentes ===\n");

    while (atual != NULL) {
        if (atual->tipoUtilizador == TIPO_GESTOR_SOFTWARE && atual->validado == 0) {
            encontrou = 1;
            printf("\nUtilizador: %s\n", atual->nomeUtilizador);
            printf("Validar este gestor? (s/n): ");
            scanf(" %c", &resposta);
            limparBufferEntrada();

            if (resposta == 's' || resposta == 'S') {
                atual->validado = 1;
                guardarUtilizadores(utilizadores);
                registarAuditoria("Validar gestor de software", -1, atual->nomeUtilizador);
                printf("Gestor validado com sucesso.\n");
            } else {
                printf("Gestor mantido pendente.\n");
            }
        }

        atual = atual->seguinte;
    }

    if (!encontrou) {
        printf("Não existem gestores pendentes.\n");
    }
}

void libertarUtilizadores(Utilizador *utilizadores) {
    Utilizador *atual = utilizadores;

    while (atual != NULL) {
        Utilizador *seguinte = atual->seguinte;
        free(atual);
        atual = seguinte;
    }
}
