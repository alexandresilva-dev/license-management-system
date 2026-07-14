#ifndef RELATORIOS_H
#define RELATORIOS_H

#include "tipos.h"

void gerarRelatorioCustos(Licenca *licencas);
void gerarRelatorioUtilizacao(Licenca *licencas, Atribuicao *atribuicoes);
void exportarLicencasCsv(Licenca *licencas);
void exportarLicencasTxt(Licenca *licencas);
void mostrarEstatisticas(Licenca *licencas, Atribuicao *atribuicoes);
void mostrarAlertasValidade(Licenca *licencas);
void registarAuditoria(const char *operacao, int idLicenca, const char *detalhes);
void mostrarHistoricoLicenca(void);

#endif
