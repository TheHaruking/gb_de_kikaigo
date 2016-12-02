rem もし、ビルドを行う場合は以下のpathを各自の環境に合わせてください。

path=C:\rgbds\bin\

set TITLE="GBdeKIKAIGO"
rgbasm -o	Fonts2.o	Fonts2.z80
rgbasm -o	%TITLE%.o	%TITLE%.z80
rgblink -m %TITLE%.map -n %TITLE%.sym -o %TITLE%.gb %TITLE%.o Fonts2.o
rgbfix -p0 -v %TITLE%.gb
