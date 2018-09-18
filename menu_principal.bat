@ECHO off
cls
:start
cls
ECHO. ------------------------
ECHO. --JAVIER ZALDIVAR COTO--
ECHO. ------------------------
ECHO. -----Menu Principal-----
ECHO. ------------------------
ECHO 1. Respaldo de un usuario
ECHO 2. Recuperacion
REM ECHO 3. Borrar Tabla
REM ECHO 4. Listar Usuarios
REM ECHO 5. Listar Objetos Test
REM ECHO 6. Crear Tablespace
REM ECHO 7. Drop Tablespace
REM ECHO 8. Insertar
ECHO 0. Salir

set choice=
set /p choice=Indique la tarea correspondiente:
if not '%choice%'=='' set choice=%choice:~0,2%
if '%choice%'=='1' goto OptRespaldos
if '%choice%'=='2' goto OptRecuperacion
if '%choice%'=='0' goto OptSalir
ECHO "%choice%" Opcion incorrecta.
pause
ECHO.
goto start

:OptRespaldos
.\menu_respaldos.bat
goto start

:OptRecuperacion
.\menu_recuperacion.bat
goto start

:OptSalir
ECHO Sale del programa
goto end


:end
