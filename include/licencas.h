#ifndef LICENCAS_H
#define LICENCAS_H

#include "tipos.h"

Licenca *carregarLicencas(void);
void guardarLicencas(Licenca *licencas);
void atualizarLicencasExpiradas(Licenca *licencas);
void adicionarLicenca(Licenca **licencas);
void listarLicencas(Licenca *licencas);
void editarLicenca(Licenca *licencas);
void removerLicenca(Licenca **licencas, Atribuicao *atribuicoes, PedidoRenovacao *pedidos);
void menuOrdenarLicencas(Licenca **licencas);
void menuPesquisarLicencas(Licenca *licencas);
void listarLicencasPorEstado(Licenca *licencas);
void libertarLicencas(Licenca *licencas);

#endif
