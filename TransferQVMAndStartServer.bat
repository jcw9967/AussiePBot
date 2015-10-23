cd build\release-mingw32-x86\base\vm
if not exist "%APPDATA%\Tremulous\Pbot4.0\vm" mkdir "%APPDATA%\Tremulous\Pbot4.0\vm"
copy game.qvm %APPDATA%\Tremulous\Pbot4.0\vm\game.qvm
mkdir vm
copy cgame.qvm vm\cgame.qvm
copy ui.qvm vm\ui.qvm
"C:\Program Files\7-Zip\7z.exe" a -tzip -aoa "%APPDATA%\Tremulous\Pbot4.0\vms.pk3" vm

"%userprofile%\Documents\Code\AussiePbot2\startserver.bat"