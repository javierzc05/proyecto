@ECHO off
cls
:start
cls
ECHO. ------------------------
ECHO. --JAVIER ZALDIVAR COTO--
ECHO. ------------------------
ECHO. -----Menu Respaldos-----
ECHO. ------------------------
ECHO 1.1 Respaldo de un usuario
ECHO 1.2 Respaldo completo Oracle
ECHO 1.3 Regresar al menu principal

set choice=
set /p choice=Indique la tarea correspondiente:
if not '%choice%'=='' set choice=%choice:~0,2%
if '%choice%'=='1' goto OptRespaldoUsuario
if '%choice%'=='2' goto OptRespaldoCompleto
if '%choice%'=='3' goto OptSalir
ECHO "%choice%" Opcion incorrecta.
pause
ECHO.
goto start

:OptRespaldoUsuario
start .\respaldo_usuario.bat
goto start

:OptRespaldoCompleto
start .\respaldo_completo.bat
goto start

:OptSalir
.\menu_principal.bat
goto end

:end
