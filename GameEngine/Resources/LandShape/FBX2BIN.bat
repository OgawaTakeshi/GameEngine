@echo off

set EXT=.landshape
set CNV=FBX2BIN.exe

if "%~1"=="" goto HELP

echo landshape�t�@�C���𐶐����܂�

REM fbx�t�@�C���̂���h���C�u�A�f�B���N�g���Ɉړ�
%~d1
cd %~dp1
REM �R���o�[�^�ŕϊ�
%~dp0%CNV% %1
REM �t�@�C�����o�b�`�̂���f�B���N�g���Ɉړ�
move %~n1%EXT% %~dp0

goto END

:HELP
echo FBX�t�@�C�����h���b�O���Ă�������
pause

:END
