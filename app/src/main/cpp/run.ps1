<#
#Raylib Automated Runner by Limon DAS
#Based on a youtube video:building raylib from scratch
==========================
||       TUTORIAL       ||
==========================
run `./run.ps1` to compile `./main.c` to `game.exe`,it will remove previous `game.exe` if exits.then run the `game.exe`
adding p flag : `./run.ps1 will compile `main.c` to `game.exe` without terminal log for production.
-------------------------
LAST UPDATE: 09/03/2026 
#>
param(
    [switch]$p
)
if(Test-Path game.exe){
    rm game.exe
}
if($p){
    #running in  prod mode without terminal log: ./run.ps1 -p
gcc main.c -o game.exe -O1 -Wall -std=c99 -Wno-missing-braces -I include/ -L lib/ -lraylib -lopengl32 -lgdi32 -lwinmm -mwindows
}else{
    gcc main.c -o game.exe -O1 -Wall -std=c99 -Wno-missing-braces -I include/ -L lib/ -lraylib -lopengl32 -lgdi32 -lwinmm
}
if($?){
    ./game.exe
}else{
    Write-Host "Error!"
}