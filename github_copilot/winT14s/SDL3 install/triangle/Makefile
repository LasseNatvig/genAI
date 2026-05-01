SDL3_INC = ..\SDL3\include
SDL3_LIB = ..\SDL3\lib\x64\SDL3.lib
SDL3_DLL = ..\SDL3\lib\x64\SDL3.dll

CC     = cl
CFLAGS = /nologo /W3 /I"$(SDL3_INC)" /DSDL_MAIN_HANDLED /Zi /Od /Fd:bin\triangle.pdb

all:
	@if not exist bin mkdir bin
	$(CC) $(CFLAGS) triangleplain.c /Fe:bin\triangle.exe /link $(SDL3_LIB) /DEBUG /PDB:bin\triangle.pdb
	@copy /Y "$(SDL3_DLL)" "bin\SDL3.dll" >nul
	@echo Done: bin\triangle.exe

clean:
	@if exist bin rmdir /s /q bin
