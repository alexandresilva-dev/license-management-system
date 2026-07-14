#ifndef UTILITARIOS_H
#define UTILITARIOS_H

#include "tipos.h"

int prepararPastasProjeto(void);
void limparBufferEntrada(void);
int lerInteiro(const char *mensagem);
float lerFloat(const char *mensagem);
void lerTexto(const char *mensagem, char *destino, int tamanho);
void lerTextoObrigatorio(const char *mensagem, char *destino, int tamanho);
Data lerData(const char *mensagem);
int dataValida(Data data);
void mostrarData(Data data);
int compararDatas(Data primeira, Data segunda);
Data obterDataAtual(void);

#endif
