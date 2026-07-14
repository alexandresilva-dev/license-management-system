#ifndef UTILIZADORES_H
#define UTILIZADORES_H

#include "tipos.h"

Utilizador *carregarUtilizadores(void);
void guardarUtilizadores(Utilizador *utilizadores);
void garantirAdministradorInicial(Utilizador **utilizadores);
Utilizador *iniciarSessaoOuRegistar(Utilizador **utilizadores);
void registarGestorSoftware(Utilizador **utilizadores);
Utilizador *procurarUtilizador(Utilizador *utilizadores, const char *nomeUtilizador);
void validarGestoresSoftware(Utilizador *utilizadores);
void libertarUtilizadores(Utilizador *utilizadores);

#endif
