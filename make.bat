@echo off
rem ビルドを行う場合は以下の PATH を各自の環境に合わせてください。
set PATH=PATH;
set TITLE="GBdeKIKAIGO"

rgbasm -o obj/Fonts2.o src/Fonts2.asm
rgbasm -o obj/%TITLE%.o src/%TITLE%.asm
rgblink -m obj/%TITLE%.map -n %TITLE%.sym -o %TITLE%.gbc obj/%TITLE%.o obj/Fonts2.o
rgbfix -p0 -v %TITLE%.gbc
