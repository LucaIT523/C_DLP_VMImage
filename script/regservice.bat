@echo off
setlocal

set SERVICE_NAME=KenanAgentService
set EXECUTABLE=%~dp0agent_svc.exe

sc create "%SERVICE_NAME%" binPath= "%EXECUTABLE%" start= auto

sc start "%SERVICE_NAME%"

echo "reg ... ok"

endlocal