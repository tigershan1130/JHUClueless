set TestName=TestBadKill
set MapName=/Game/GamePlays/Maps/MapTest2
set TestDuration=60
set runtests_log="..\Saved\Logs\RunTests.log"

rem del runtests.log file
if exist "%runtests_log%" (
    del /q /f "%runtests_log%"
)

if "%RUN_BAT_TITLE%" == "" (
    rem set default windows title
    set run_bat_title=autotest_bat_default_title
) else (
    rem RUN_BAT_TITLE don't use double quotation marks
    set run_bat_title=%RUN_BAT_TITLE%
)

rem run_bat_title must be use double quotation marks
start "%run_bat_title%" AutomationTest_DedicatedServer.bat %1=%MapName%

ping 127.0.0.1 -n 10 >"%tmp%\null"

start "%run_bat_title%" AutomationTest_ClientPartner.bat

ping 127.0.0.1 -n 10 >"%tmp%\null"

start "%run_bat_title%" AutomationTest_ClientTest.bat %1=%MapName%.%TestName%

set _wait_second=0
:wait_loop
    if exist "%runtests_log%" (
        for /F %%j in ( 'findstr /i /c:"]Log file closed," "%runtests_log%"' ) do (
            set log_closed_str=%%j
        )
    )
    rem log_closed_str must be use double quotation marks
    if not "%log_closed_str%" == "" (
        goto taskkill_bat_and_unreal
    )
    ping 127.0.0.1 -n 11 >"%tmp%\null"
    set /a "_wait_second=_wait_second+10"
    if %_wait_second%==300 goto timeout_step
    goto wait_loop

:timeout_step
    echo "Testname: %TestName% 300s timeout"
    goto taskkill_bat_and_unreal

rem must be at the end
:taskkill_bat_and_unreal
    taskkill /f /fi "windowtitle eq %run_bat_title%*"
    taskkill /f /im UnrealEditor.exe
    exit 0
