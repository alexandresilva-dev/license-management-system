#!/bin/zsh

DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$DIR"

mkdir -p dados relatorios

if [[ ! -x sistema_licencas ]] || [[ Makefile -nt sistema_licencas ]] || \
   find src include -type f -newer sistema_licencas -print -quit | grep -q .; then
    echo "A compilar o Sistema de Gestão de Licenças..."
    if ! make; then
        echo
        echo "Não foi possível compilar o programa."
        echo "Prima ENTER para fechar esta janela..."
        read
        exit 1
    fi
    echo
fi

./sistema_licencas

echo
echo "Prima ENTER para fechar esta janela..."
read || true
exit 0
