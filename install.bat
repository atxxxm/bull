@echo off
g++ src\main.cpp src\bull.cpp src\slog.cpp -o bull.exe
icacls bull.exe /grant Everyone:RX
if not exist "%ProgramFiles%\Bull" mkdir "%ProgramFiles%\Bull"
move /Y bull.exe "%ProgramFiles%\Bull\"
setx PATH "%PATH%;%ProgramFiles%\Bull\" /m