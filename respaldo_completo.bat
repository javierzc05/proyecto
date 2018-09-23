@ECHO off
cls
exp parfile=.\respaldos\completo.par

REM Comprime
.\apps\7z.exe a .\temporal\completo.7z .\respaldos\completo.dmp 1>.\logs\comprime_completo.log 2>.\logs\comprime_completo.err

REM Encripta
.\apps\aescrypt.exe -e -p clave123 .\temporal\completo.7z 1>.\logs\encripta_completo.log 2>.\logs\encripta_completo.err

REM Namedate
.\apps\namedate.exe /Y /ZZ:"F(Y-m-d)-(H-M-S).X" .\temporal\completo.7z.aes 1>.\logs\namedate_completo.log 2>.\logs\namedate_completo.err

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
echo #0 Exportar completo                                   >> .\temporal\respaldo.log
type .\logs\completo.log                                    >> .\temporal\respaldo.log
echo Final exportar ======================================= >> .\temporal\respaldo.log
echo.                                                       >> .\temporal\respaldo.log
echo #1 Comprimir el archivo                                >> .\temporal\respaldo.log
type .\logs\comprime_completo.log                           >> .\temporal\respaldo.log
echo error:                                                 >> .\temporal\respaldo.log
type .\logs\comprime_completo.err                           >> .\temporal\respaldo.log
echo Final comprimir ====================================== >> .\temporal\respaldo.log
echo.                                                       >> .\temporal\respaldo.log
echo #2 Encriptar el archivo                                >> .\temporal\respaldo.log
type .\logs\encripta_completo.log                           >> .\temporal\respaldo.log
echo error:                                                 >> .\temporal\respaldo.log
type .\logs\encripta_completo.err                           >> .\temporal\respaldo.log
echo Final encriptar ====================================== >> .\temporal\respaldo.log
echo.                                                       >> .\temporal\respaldo.log
echo #3 Renombrar el archivo                                >> .\temporal\respaldo.log
type .\logs\namedate_completo.log                           >> .\temporal\respaldo.log
echo error:                                                 >> .\temporal\respaldo.log
type .\logs\namedate_completo.err                           >> .\temporal\respaldo.log
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
del .\respaldos\completo.dmp
del .\respaldos\completo.log
del .\temporal\completo.7z
exit