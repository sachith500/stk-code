find_path(ANGELSCRIPT_INCLUDE_DIR NAMES angelscript/include/angelscript.h PATHS "${PROJECT_SOURCE_DIR}/dependencies/include")

find_library(ANGELSCRIPT_LIBRARY NAMES angelscriptd PATHS "${PROJECT_SOURCE_DIR}/dependencies/lib")
