#ifndef TIPOS_H
#define TIPOS_H

#define MAX_NOME_UTILIZADOR 50
#define MAX_PALAVRA_PASSE 50
#define MAX_NOME 100
#define MAX_CHAVE 100

typedef enum {
    TIPO_ADMINISTRADOR = 1,
    TIPO_GESTOR_SOFTWARE = 2
} TipoUtilizador;

typedef enum {
    LICENCA_PERPETUA = 1,
    LICENCA_ANUAL = 2,
    LICENCA_OEM = 3,
    LICENCA_EDUCACIONAL = 4
} TipoLicenca;

typedef enum {
    ESTADO_ATIVA = 1,
    ESTADO_EXPIRADA = 2,
    ESTADO_SUSPENSA = 3,
    ESTADO_REVOGADA = 4
} EstadoLicenca;

typedef struct {
    int dia;
    int mes;
    int ano;
} Data;

typedef struct utilizador {
    char nomeUtilizador[MAX_NOME_UTILIZADOR];
    char palavraPasse[MAX_PALAVRA_PASSE];
    TipoUtilizador tipoUtilizador;
    int validado;
    int primeiroAcesso;
    struct utilizador *seguinte;
} Utilizador;

typedef struct licenca {
    int id;
    char nomeProduto[MAX_NOME];
    char fabricante[MAX_NOME];
    TipoLicenca tipoLicenca;
    char chaveAtivacao[MAX_CHAVE];
    int postosPermitidos;
    Data dataAquisicao;
    Data dataValidade;
    float custo;
    EstadoLicenca estado;
    struct licenca *seguinte;
} Licenca;

typedef struct atribuicao {
    int id;
    int idLicenca;
    char identificadorPosto[MAX_NOME];
    char responsavel[MAX_NOME];
    Data dataInstalacao;
    Data dataDesinstalacao;
    int ativa;
    struct atribuicao *seguinte;
} Atribuicao;

typedef enum {
    RENOVACAO_PENDENTE = 1,
    RENOVACAO_APROVADA = 2,
    RENOVACAO_REJEITADA = 3
} EstadoRenovacao;

typedef struct pedido_renovacao {
    int id;
    int idLicenca;
    char pedidoPor[MAX_NOME_UTILIZADOR];
    Data dataPedido;
    EstadoRenovacao estado;
    struct pedido_renovacao *seguinte;
} PedidoRenovacao;

#endif
