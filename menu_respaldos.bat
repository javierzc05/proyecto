@ECHO off
cls
:start
cls
ECHO. --Menu Respaldos---
ECHO 1. Respaldo de un usuario
REM ECHO 2. Crear Tabla
REM ECHO 3. Borrar Tabla
REM ECHO 4. Listar Usuarios
REM ECHO 5. Listar Objetos Test
REM ECHO 6. Crear Tablespace
REM ECHO 7. Drop Tablespace
REM ECHO 8. Insertar
ECHO 0. Regresar al menu principal

set choice=
set /p choice=Indique la tarea correspondiente:
if not '%choice%'=='' set choice=%choice:~0,2%
if '%choice%'=='1' goto OptRespaldoUsuario
REM if '%choice%'=='2' goto OptCreaTabla
REM if '%choice%'=='3' goto OptBorraTabla
REM if '%choice%'=='4' goto OptListarUsuarios
REM if '%choice%'=='5' goto OptListarObjetosTest
REM if '%choice%'=='6' goto OptCrearTablespace
REM if '%choice%'=='7' goto OptDropTablespace
REM if '%choice%'=='8' goto OptInsertar
if '%choice%'=='0' goto OptSalir
ECHO "%choice%" Opcion incorrecta.
pause
ECHO.
goto start

:OptRespaldoUsuario
start .\respaldo_usuario.bat
goto start


:OptSalir
.\menu_principal.bat
goto end

:end
