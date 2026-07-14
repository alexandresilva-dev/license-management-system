#ifndef ATRIBUICOES_H
#define ATRIBUICOES_H

#include "tipos.h"

Atribuicao *carregarAtribuicoes(void);
void guardarAtribuicoes(Atribuicao *atribuicoes);
void registarInstalacao(Atribuicao **atribuicoes, Licenca *licencas, const char *responsavelFixo);
void registarDesinstalacao(Atribuicao *atribuicoes, const char *responsavelPermitido);
void listarAtribuicoes(Atribuicao *atribuicoes);
void listarAtribuicoesPorResponsavel(Atribuicao *atribuicoes, Licenca *licencas, const char *responsavel);
void libertarAtribuicoes(Atribuicao *atribuicoes);

#endif
