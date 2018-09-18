@ECHO off
cls




set Usuario=
set /p Usuario=Indique el Usuario que desea recuperar:
 


REM REM Acomoda descendientemente
REM REM DIR /A-D /OD .\respaldos\juan*.aes
REM FOR /f "tokens=*" %%G IN ('DIR /A-D /OD .\respaldos\juan*.aes') DO echo %%G
for /f "delims=" %%F in ('dir .\respaldos\%Usuario%*.aes /b /o-n') do set archivo=%%F
echo %archivo%



REM Se desencripta el archivo
.\apps\aescrypt.exe -d -p clave123 .\respaldos\%archivo% 1>.\logs\desencripta.log 2>.\logs\desencripta.err

REM Se descomprime el archivo
.\apps\7z.exe e .\respaldos\%Usuario%*.7z -y -o.\respaldos 1>.\logs\descomprime.log 2>.\logs\descomprime.err

REM Eliminar
del .\respaldos\%Usuario%*.7z

set Usuario2=
set /p Usuario2=Indique el Usuario destino:

echo userid=system/root > .\respaldos\imp-%Usuario%.par
echo fromuser=%Usuario%  >>.\respaldos\imp-%Usuario%.par
echo touser=%Usuario2% >>.\respaldos\imp-%Usuario%.par
echo file=.\respaldos\%Usuario%.dmp >>.\respaldos\imp-%Usuario%.par
echo log=.\respaldos\imp-%Usuario%.log >>.\respaldos\imp-%Usuario%.par
imp parfile=.\respaldos\imp-%Usuario%.par
pause
exit
