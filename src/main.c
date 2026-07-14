#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "utilitarios.h"
#include "utilizadores.h"
#include "licencas.h"
#include "atribuicoes.h"
#include "relatorios.h"
#include "renovacoes.h"

static void menuAdministrador(Utilizador *utilizadores, Licenca **licencas, Atribuicao **atribuicoes, PedidoRenovacao **pedidosRenovacao);
static void menuGestorSoftware(Utilizador *utilizador, Licenca *licencas, Atribuicao **atribuicoes, PedidoRenovacao **pedidosRenovacao);

static void usarPastaDoExecutavel(const char *caminhoExecutavel) {
    char caminho[1024];
    char *ultimaBarra;

    if (caminhoExecutavel == NULL || strchr(caminhoExecutavel, '/') == NULL) {
        return;
    }

    snprintf(caminho, sizeof(caminho), "%s", caminhoExecutavel);
    ultimaBarra = strrchr(caminho, '/');

    if (ultimaBarra == caminho) {
        ultimaBarra[1] = '\0';
    } else {
        *ultimaBarra = '\0';
    }

    chdir(caminho);
}

int main(int argc, char *argv[]) {
    int opcao;
    Utilizador *utilizadores;
    Licenca *licencas;
    Atribuicao *atribuicoes;
    PedidoRenovacao *pedidosRenovacao;
    Utilizador *utilizadorAutenticado;

    if (argc > 0) {
        usarPastaDoExecutavel(argv[0]);
    }

    if (!prepararPastasProjeto()) {
        return 1;
    }

    utilizadores = carregarUtilizadores();
    licencas = carregarLicencas();
    atribuicoes = carregarAtribuicoes();
    pedidosRenovacao = carregarPedidosRenovacao();

    garantirAdministradorInicial(&utilizadores);
    atualizarLicencasExpiradas(licencas);

    do {
        printf("\n=== Sistema de Gestão de Licenças ===\n");
        printf("1. Iniciar sessão\n");
        printf("2. Registar Gestor de Software\n");
        printf("0. Sair\n");

        opcao = lerInteiro("Opção: ");

        switch (opcao) {
            case 1:
                utilizadorAutenticado = iniciarSessaoOuRegistar(&utilizadores);

                if (utilizadorAutenticado != NULL) {
                    if (utilizadorAutenticado->tipoUtilizador == TIPO_ADMINISTRADOR) {
                        menuAdministrador(utilizadores, &licencas, &atribuicoes, &pedidosRenovacao);
                    } else {
                        menuGestorSoftware(utilizadorAutenticado, licencas, &atribuicoes, &pedidosRenovacao);
                    }
                }
                break;

            case 2:
                registarGestorSoftware(&utilizadores);
                break;

            case 0:
                printf("\nA sair do programa...\n");
                guardarLicencas(licencas);
                guardarAtribuicoes(atribuicoes);
                guardarPedidosRenovacao(pedidosRenovacao);
                guardarUtilizadores(utilizadores);
                break;

            default:
                printf("\nOpção inválida.\n");
        }
    } while (opcao != 0);

    libertarUtilizadores(utilizadores);
    libertarLicencas(licencas);
    libertarAtribuicoes(atribuicoes);
    libertarPedidosRenovacao(pedidosRenovacao);

    return 0;
}

static void menuAdministrador(Utilizador *utilizadores, Licenca **licencas, Atribuicao **atribuicoes, PedidoRenovacao **pedidosRenovacao) {
    int opcao;

    do {
        printf("\n=== Menu Administrador ===\n");
        printf("1. Validar Gestores de Software\n");
        printf("2. Adicionar Licença\n");
        printf("3. Listar Licenças\n");
        printf("4. Editar Licença\n");
        printf("5. Remover Licença\n");
        printf("6. Ordenar Licenças\n");
        printf("7. Registar Instalação\n");
        printf("8. Registar Desinstalação\n");
        printf("9. Listar Atribuições\n");
        printf("10. Relatório de Custos\n");
        printf("11. Relatório de Utilização\n");
        printf("12. Exportar Licenças CSV\n");
        printf("13. Exportar Licenças TXT\n");
        printf("14. Alertas de Expiração\n");
        printf("15. Estatísticas\n");
        printf("16. Gerir Pedidos de Renovação\n");
        printf("17. Listar Pedidos de Renovação\n");
        printf("18. Histórico de Licença\n");
        printf("19. Pesquisar Licenças\n");
        printf("20. Listar Licenças por Estado\n");
        printf("0. Terminar sessão\n");

        opcao = lerInteiro("Opção: ");

        switch (opcao) {
            case 1:
                validarGestoresSoftware(utilizadores);
                break;
            case 2:
                adicionarLicenca(licencas);
                break;
            case 3:
                listarLicencas(*licencas);
                break;
            case 4:
                editarLicenca(*licencas);
                break;
            case 5:
                removerLicenca(licencas, *atribuicoes, *pedidosRenovacao);
                break;
            case 6:
                menuOrdenarLicencas(licencas);
                break;
            case 7:
                registarInstalacao(atribuicoes, *licencas, NULL);
                break;
            case 8:
                registarDesinstalacao(*atribuicoes, NULL);
                break;
            case 9:
                listarAtribuicoes(*atribuicoes);
                break;
            case 10:
                gerarRelatorioCustos(*licencas);
                break;
            case 11:
                gerarRelatorioUtilizacao(*licencas, *atribuicoes);
                break;
            case 12:
                exportarLicencasCsv(*licencas);
                break;
            case 13:
                exportarLicencasTxt(*licencas);
                break;
            case 14:
                mostrarAlertasValidade(*licencas);
                break;
            case 15:
                mostrarEstatisticas(*licencas, *atribuicoes);
                break;
            case 16:
                gerirPedidosRenovacao(*pedidosRenovacao, *licencas);
                break;
            case 17:
                listarPedidosRenovacao(*pedidosRenovacao);
                break;
            case 18:
                mostrarHistoricoLicenca();
                break;
            case 19:
                menuPesquisarLicencas(*licencas);
                break;
            case 20:
                listarLicencasPorEstado(*licencas);
                break;
            case 0:
                printf("Sessão terminada.\n");
                break;
            default:
                printf("Opção inválida.\n");
        }
    } while (opcao != 0);
}

static void menuGestorSoftware(Utilizador *utilizador, Licenca *licencas, Atribuicao **atribuicoes, PedidoRenovacao **pedidosRenovacao) {
    int opcao;

    do {
        printf("\n=== Menu Gestor de Software: %s ===\n", utilizador->nomeUtilizador);
        printf("1. Consultar Licenças Atribuídas\n");
        printf("2. Registar Instalação\n");
        printf("3. Registar Desinstalação\n");
        printf("4. Listar as Minhas Atribuições\n");
        printf("5. Estatísticas\n");
        printf("6. Solicitar Renovação\n");
        printf("7. Listar os Meus Pedidos de Renovação\n");
        printf("8. Histórico de Licença\n");
        printf("9. Pesquisar Licenças\n");
        printf("0. Terminar sessão\n");

        opcao = lerInteiro("Opção: ");

        switch (opcao) {
            case 1:
                listarAtribuicoesPorResponsavel(*atribuicoes, licencas, utilizador->nomeUtilizador);
                break;
            case 2:
                registarInstalacao(atribuicoes, licencas, utilizador->nomeUtilizador);
                break;
            case 3:
                registarDesinstalacao(*atribuicoes, utilizador->nomeUtilizador);
                break;
            case 4:
                listarAtribuicoesPorResponsavel(*atribuicoes, licencas, utilizador->nomeUtilizador);
                break;
            case 5:
                mostrarEstatisticas(licencas, *atribuicoes);
                break;
            case 6:
                solicitarRenovacao(pedidosRenovacao, licencas, utilizador->nomeUtilizador);
                break;
            case 7:
                listarPedidosRenovacaoPorUtilizador(*pedidosRenovacao, utilizador->nomeUtilizador);
                break;
            case 8:
                mostrarHistoricoLicenca();
                break;
            case 9:
                menuPesquisarLicencas(licencas);
                break;
            case 0:
                printf("Sessão terminada.\n");
                break;
            default:
                printf("Opção inválida.\n");
        }
    } while (opcao != 0);
}
