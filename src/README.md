Currently, running the .bat file in cmd.exe works for me, and running the .ps1 in VS code for some reason does not.

There are a few things where I am not sure why they are happening:

If I try to run .\run_windows_SDL.ps1 main_sdl.c in a powershell terminal inside of VS Code, it gives the following error:

ParserError: C:\repos\game4_sdl\run_windows_SDL.ps1:32:272
Line |
  32 |  … 2-2.0.12\x86_64-w64-mingw32\lib Libraries\glad\glad.c -w -Wl,-subsyst …
     |                                                                ~
     | Missing argument in parameter list.

I could try in ordinary powershell, not in VS Code. Perhaps I need to be using the VS Code version of SDL instead of the mingw version? That seems weird to me but maybe.

So I compile it in the cmd.exe command line with the following:

gcc main_sdl.c -ILibraries\glad\include -IC:\Users\fdhgg\Downloads\SDL2-devel-2.0.12-mingw\SDL2-devel-2.0\SDL2-2.0.12\x86_64-w64-mingw32\include -LC:\Users\fdhgg\Downloads\SDL2-devel-2.0.12-mingw\SDL2-devel-2.0\SDL2-2.0.12\x86_64-w64-mingw32\lib Libraries\glad\glad.c -w -Wl,-subsystem,windows -lmingw32 -lSDL2main -lSDL2 -o a.exe

And then run it with just a.exe

Maybe I should use a batch file. Not sure.

