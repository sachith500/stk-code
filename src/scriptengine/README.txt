This is currently only a tiny prototype to get an idea of what the scripting engine can do/ check it's performance.
TODO:
set up a build target for angelscript
get file manager support for loading scripts
move scripts to stk-data, etc.


Steps to get this working.
1)get AngelScript SDK 2.28.1

http://www.angelcode.com/angelscript/downloads.html

2)Build the AngelScript library
IF LINUX{

Build AngelScript by going to 

cd ANGELSCRIPT_DIR\sdk\angelscript\projects\gnuc
make

==> angelscript.a
}

IF WINDOWS{

Load the VS Project, Build the .lib

==>angelscriptd.lib
}

Copy the generated library file (.a or .lib) to stk-code/dependencies/lib
Comment/Uncomment the lines in CMakeLists in stk-code as necessary (for Linux/Windows)

Build the game.
Test 


