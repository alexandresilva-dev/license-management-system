@echo off
chcp 65001 >nul
title Sistema de Gestão de Licenças

cd /d "%~dp0"

if not exist "dados" mkdir dados
if not exist "relatorios" mkdir relatorios

if not exist "sistema_licencas.exe" (
    echo ERRO: O ficheiro sistema_licencas.exe nao foi encontrado.
    echo Certifique-se de que o ficheiro esta na mesma pasta que este script.
    echo.
    pause
    exit /b 1
)

sistema_licencas.exe

echo.
echo Prima qualquer tecla para fechar esta janela...
pause >nul
