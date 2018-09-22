@ECHO off
cls
exp parfile=.\respaldos\completo.par

REM Comprime
.\apps\7z.exe a .\respaldos\completo.7z .\respaldos\completo.log .\respaldos\completo.dmp 1>.\logs\comprime_completo.log 2>.\logs\comprime_completo.err

REM Encripta
.\apps\aescrypt.exe -e -p clave123 .\respaldos\completo.7z 1>.\logs\encripta_completo.log 2>.\logs\encripta_completo.err

REM Namedate
.\apps\namedate.exe /Y /ZZ:"F(Y-m-d)-(H-M-S).X" .\respaldos\completo.7z.aes 1>.\logs\namedate_completo.log 2>.\logs\namedate_completo.err

REM Elimina archivos restantes
del .\respaldos\completo.dmp
del .\respaldos\completo.log
del .\respaldos\completo.7z
exit