echo off
set MapName=%1
"%UE5_ENGINE_PATH%\Engine\Binaries\Win64\UnrealEditor.exe" "%UE5_PROJECT_PATH%\CluelessJHU.uproject" %MapName% -server -log -port=8888 -Test=true
goto :eof