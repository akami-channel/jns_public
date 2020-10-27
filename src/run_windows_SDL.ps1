
# this requires the file to be compiled to be received as an argument

# if no argument is provided, we echo an error message and exit
if ($args.count -eq 0) {
	echo "This script requires an argument."
	echo "Please provide the file you wish to be compiled as an argument."
	exit 1
}

# We silently move a.exe to .a.exe if a.exe exists
$pathOfExecutable = $PSScriptRoot + "\a.exe"
if( [System.IO.File]::Exists($pathOfExecutable) ) 
{
	#silent rm
	Get-ChildItem -Path .a.exe -Recurse | Remove-Item -force -recurse
	mv a.exe .a.exe
}

# compilation and linking
# -I..\..\Libraries\glad\include\KHR
# gcc $args[0] -ILibraries\glad\include Libraries\GLFW\Windows\libglfw3.a Libraries\glad\glad.c -lopengl32 -lgdi32
#gcc $args[0] -ILibraries\glad\include -IC:\Users\fdhgg\Downloads\SDL2-devel-2.0.12-VC\SDL2-2.0.12\include -LC:\Users\fdhgg\Downloads\SDL2-devel-2.0.12-VC\SDL2-2.0.12\lib Libraries\glad\glad.c -w -Wl,-subsystem,windows -lmingw32 -lSDL2main -lSDL2 -lopengl32 -lgdi32 -o a.exe

# I am currently running this in cmd:
# (for only object files)
#gcc -c main2.c -ILibraries\glad\include -IC:\Users\fdhgg\Downloads\SDL2-devel-2.0.12-VC\SDL2-2.0.12\include -LC:\Users\fdhgg\Downloads\SDL2-devel-2.0.12-VC\SDL2-2.0.12\lib Libraries\glad\glad.c -w -Wl,-subsystem,windows -lmingw32 -lSDL2main -lSDL2
# gcc $args[0] -ILibraries\glad\include -IC:\Users\fdhgg\Downloads\SDL2-devel-2.0.12-VC\SDL2-2.0.12\include -LC:\Users\fdhgg\Downloads\SDL2-devel-2.0.12-VC\SDL2-2.0.12\lib Libraries\glad\glad.c -w -Wl,-subsystem,windows -lmingw32 -lSDL2main -lSDL2 -o a.exe

# Currently running this in cmd:
# gcc main2.c -ILibraries\glad\include -IC:\Users\fdhgg\Downloads\SDL2-devel-2.0.12-mingw\SDL2-devel-2.0\SDL2-2.0.12\x86_64-w64-mingw32\include -LC:\Users\fdhgg\Downloads\SDL2-devel-2.0.12-mingw\SDL2-devel-2.0\SDL2-2.0.12\x86_64-w64-mingw32\lib Libraries\glad\glad.c -w -Wl,-subsystem,windows -lmingw32 -lSDL2main -lSDL2 -o a.exe
gcc $args[0] -ILibraries\glad\include -IC:\Users\fdhgg\Downloads\SDL2-devel-2.0.12-mingw\SDL2-devel-2.0\SDL2-2.0.12\x86_64-w64-mingw32\include -LC:\Users\fdhgg\Downloads\SDL2-devel-2.0.12-mingw\SDL2-devel-2.0\SDL2-2.0.12\x86_64-w64-mingw32\lib Libraries\glad\glad.c -w -Wl,-subsystem,windows -lmingw32 -lSDL2main -lSDL2 -o a.exe

# execute
.\a.exe
