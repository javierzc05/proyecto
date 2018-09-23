@ECHO off
cls
:start
cls
ECHO. ------------------------
ECHO. --JAVIER ZALDIVAR COTO--
ECHO. ------------------------
ECHO. ---Menu Recuperacion----
ECHO. ------------------------
ECHO 2.1 Recuperacion de un usuario
ECHO 2.2 Regresar al menu principal

set choice=
set /p choice=Indique la tarea correspondiente:
if not '%choice%'=='' set choice=%choice:~0,2%
if '%choice%'=='1' goto OptRecuperacionUsuario
if '%choice%'=='2' goto OptSalir
if '%choice%'=='a' goto OptRecuperacionUsuario
if '%choice%'=='b' goto OptSalir
ECHO "%choice%" Opcion incorrecta.
pause
ECHO.
goto start

:OptRecuperacionUsuario
start .\recuperar_usuario.bat
goto start


:OptSalir
.\menu_principal.bat
goto end

:end
