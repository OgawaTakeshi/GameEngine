@echo off

set EXT=.landshape
set CNV=FBX2BIN.exe

if "%~1"=="" goto HELP

echo landshapeファイルを生成します

REM fbxファイルのあるドライブ、ディレクトリに移動
%~d1
cd %~dp1
REM コンバータで変換
%~dp0%CNV% %1
REM ファイルをバッチのあるディレクトリに移動
move %~n1%EXT% %~dp0

goto END

:HELP
echo FBXファイルをドラッグしてください
pause

:END
