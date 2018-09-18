@ECHO off
cls

set Usuario=
set /p Usuario=Indique el Usuario a Filtrar:

echo userid=system/root > .\respaldos\%Usuario%.par
echo owner=%Usuario%  >> .\respaldos\%Usuario%.par
echo file=.\respaldos\%Usuario%.dmp >> .\respaldos\%Usuario%.par
echo log=.\respaldos\%Usuario%.log >> .\respaldos\%Usuario%.par
echo statistics=none >> .\respaldos\%Usuario%.par

exp parfile=.\respaldos\%Usuario%.par

REM Comprime
.\apps\7z.exe a .\respaldos\%Usuario%.7z .\respaldos\%Usuario%.log .\respaldos\%Usuario%.dmp 1>.\logs\comprime.log 2>.\logs\comprime.err

REM Encripta
.\apps\aescrypt.exe -e -p clave123 .\respaldos\%Usuario%.7z 1>.\logs\encripta.log 2>.\logs\encripta.err

REM Namedate
.\apps\namedate.exe /Y /ZZ:"F(Y-m-d)-(H-M-S).X" .\respaldos\%Usuario%.7z.aes 1>.\logs\namedate.log 2>.\logs\namedate.err

REM Elimina archivos restantes
del .\respaldos\%Usuario%.par
del .\respaldos\%Usuario%.dmp
del .\respaldos\%Usuario%.log
del .\respaldos\%Usuario%.7z

exit