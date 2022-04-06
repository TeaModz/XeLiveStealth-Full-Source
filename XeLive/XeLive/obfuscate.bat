pushd %~dp0
@echo off
start decrypt.bat
TIMEOUT /T 2 /NOBREAK
start XeLive.Obfuscation.exe
TIMEOUT /T 2 /NOBREAK
start encrypt.bat
TIMEOUT /T 2 /NOBREAK
del /S decrypted_XeLive.xex
del /S output.xex
del /S rc4_key_dec.bin
del /S text.bin
ren XeLive.xex XeLiveNoFuscate.xex
TIMEOUT /T 2 /NOBREAK