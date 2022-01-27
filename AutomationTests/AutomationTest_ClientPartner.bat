set ClientType=%1
echo off
"%UE5_ENGINE_PATH%\Engine\Binaries\Win64\UnrealEditor.exe" "%UE5_PROJECT_PATH%\CluelessJHU.uproject" -game 127.0.0.1:8888?Type=%ClientType%  -ResX=400 -ResY=300 -WINDOWED -nopause -unattended -nullRHI
goto :eof