@echo off
rem �r���h���s���ꍇ�͈ȉ��� PATH ���e���̊��ɍ��킹�Ă��������B
set PATH=PATH;
set TITLE="GBdeKIKAIGO"

rgbasm -o obj/Fonts2.o src/Fonts2.asm
rgbasm -o obj/%TITLE%.o src/%TITLE%.asm
rgblink -m obj/%TITLE%.map -n %TITLE%.sym -o %TITLE%.gbc obj/%TITLE%.o obj/Fonts2.o
rgbfix -p0 -v %TITLE%.gbc
