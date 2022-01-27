set TestName=%1
set ClientType=%2
"%UE5_ENGINE_PATH%\Engine\Binaries\Win64\UnrealEditor.exe" "%UE5_PROJECT_PATH%\CluelessJHU.uproject"  127.0.0.1:8888?Type=%ClientType% -game  -ResX=800 -ResY=600 -WINDOWED -ExecCmds="AutoMation RunTests %TestName%" -ReportExportPath="%ReportExportPath%" -Log -Log=RunTests.log -unattended -nopause  -testexit="Automation Test Queue Empty" -TestLoops=1 -nullRHI
exit