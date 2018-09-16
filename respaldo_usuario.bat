@ECHO off
cls
REM notar el EXIT, sale del SO

set Usuario=
set /p Usuario=Indique el Usuario a Filtrar:

echo userid=system/root > %Usuario%.par
echo owner=%Usuario%  >> %Usuario%.par
echo file=%Usuario%.dmp >> %Usuario%.par
echo log=%Usuario%.log >> %Usuario%.par
echo statistics=none >> %Usuario%.par

exp parfile=%Usuario%.par

REM Comprime
.\apps\7z.exe a .\respaldos\%Usuario%.7z .\%Usuario%.log 1>.\logs\%Usuario%_comprime.log 2>.\logs\%Usuario%_comprime.err

REM Encripta
.\apps\aescrypt.exe -e -p clave123 .\respaldos\%Usuario%.7z 1>.\logs\%Usuario%_encripta.log 2>.\logs\%Usuario%_encripta.err

REM Namedate
.\apps\namedate.exe /Y /ZZ:"F(Y-m-d)-(H-M-S).X" .\respaldos\%Usuario%.7z.aes 1>.\logs\%Usuario%_namedate.log 2>.\logs\%Usuario%_namedate.err

exit