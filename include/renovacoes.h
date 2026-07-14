#ifndef RENOVACOES_H
#define RENOVACOES_H

#include "tipos.h"

PedidoRenovacao *carregarPedidosRenovacao(void);
void guardarPedidosRenovacao(PedidoRenovacao *pedidos);
void solicitarRenovacao(PedidoRenovacao **pedidos, Licenca *licencas, const char *nomeUtilizador);
void gerirPedidosRenovacao(PedidoRenovacao *pedidos, Licenca *licencas);
void listarPedidosRenovacao(PedidoRenovacao *pedidos);
void listarPedidosRenovacaoPorUtilizador(PedidoRenovacao *pedidos, const char *nomeUtilizador);
void libertarPedidosRenovacao(PedidoRenovacao *pedidos);

#endif
