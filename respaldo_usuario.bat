@ECHO off
cls

set Usuario=
set /p Usuario=Indique el Usuario a Filtrar:

echo userid=system/root > .\respaldos\%Usuario%.par
echo owner=%Usuario%  >> .\respaldos\%Usuario%.par
echo file=.\respaldos\%Usuario%.dmp >> .\respaldos\%Usuario%.par
echo log=.\logs\%Usuario%.log >> .\respaldos\%Usuario%.par
echo statistics=none >> .\respaldos\%Usuario%.par

exp parfile=.\respaldos\%Usuario%.par

REM Comprime
.\apps\7z.exe a .\temporal\%Usuario%.7z .\respaldos\%Usuario%.dmp 1>.\logs\comprime.log 2>.\logs\comprime.err

REM Encripta
.\apps\aescrypt.exe -e -p clave123 .\temporal\%Usuario%.7z 1>.\logs\encripta.log 2>.\logs\encripta.err

REM Namedate
.\apps\namedate.exe /Y /ZZ:"F(Y-m-d)-(H-M-S).X" .\temporal\%Usuario%.7z.aes 1>.\logs\namedate.log 2>.\logs\namedate.err

REM Guarda el nombre con namedate en una variable
dir /b .\temporal\*.aes > Nombre.ini
echo off
(
set /p NomArchivo=
) < Nombre.ini

REM Mueve el archivo a /respaldos
move .\temporal\%NomArchivo% .\respaldos\ 

REM Crea el respaldo.log
echo ====================================================== > .\temporal\respaldo.log
echo Inicio %NomArchivo% respaldar                          >> .\temporal\respaldo.log
echo ====================================================== >> .\temporal\respaldo.log
echo #0 Exportar usuario                                    >> .\temporal\respaldo.log
type .\logs\%Usuario%.log                                   >> .\temporal\respaldo.log
echo Final exportar ======================================= >> .\temporal\respaldo.log
echo.                                                       >> .\temporal\respaldo.log
echo #1 Comprimir el archivo                                >> .\temporal\respaldo.log
type .\logs\comprime.log                                    >> .\temporal\respaldo.log
echo error:                                                 >> .\temporal\respaldo.log
type .\logs\comprime.err                                    >> .\temporal\respaldo.log
echo Final comprimir ====================================== >> .\temporal\respaldo.log
echo.                                                       >> .\temporal\respaldo.log
echo #2 Encriptar el archivo                                >> .\temporal\respaldo.log
type .\logs\encripta.log                                    >> .\temporal\respaldo.log
echo error:                                                 >> .\temporal\respaldo.log
type .\logs\encripta.err                                    >> .\temporal\respaldo.log
echo Final encriptar ====================================== >> .\temporal\respaldo.log
echo.                                                       >> .\temporal\respaldo.log
echo #3 Renombrar el archivo                                >> .\temporal\respaldo.log
type .\logs\namedate.log                                    >> .\temporal\respaldo.log
echo error:                                                 >> .\temporal\respaldo.log
type .\logs\namedate.err                                    >> .\temporal\respaldo.log
echo Final renombrar ====================================== >> .\temporal\respaldo.log
echo.                                                       >> .\temporal\respaldo.log
echo ====================================================== >> .\temporal\respaldo.log
echo Final %NomArchivo% respaldar                           >> .\temporal\respaldo.log
echo ====================================================== >> .\temporal\respaldo.log
echo.                                                       >> .\temporal\respaldo.log
type .\temporal\respaldo.log                                >> .\respaldos.log

REM REM Abrir con NOTEPAD.EXE
start notepad.exe .\respaldos.log

REM Elimina archivos restantes
del .\respaldos\%Usuario%.par
del .\respaldos\%Usuario%.dmp
del .\temporal\%Usuario%.7z

exit