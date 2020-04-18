cmake_minimum_required (VERSION 3.8)

project(FarNorthEngine)

set(CMAKE_BUILD_TYPE RelWithDebInfo)
set(FAR_NORTH_ENGINE_DIR ${CMAKE_CURRENT_LIST_DIR}/)
set(SourceDir ${FAR_NORTH_ENGINE_DIR}src/)
set(DataDir ${FAR_NORTH_ENGINE_DIR}res/)
set(BuildDir ${CMAKE_CURRENT_LIST_DIR}/bin/debug/x64)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BuildDir}) 
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BuildDir}) 
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${BuildDir}) 

list(APPEND CommonCompileFlags -nologo -fp:fast -FC -GR- -EHa- -Oi -WX -W4 -wd4100 -wd4189)
list(APPEND CommonCompileDefinitions -DDEBUG_BUILD=1 -DPLATFORM_WIN32=1)
list(APPEND CommonLinkerFlags -incremental:no -opt:ref)
list(APPEND CommonLinkerLibs user32.lib gdi32.lib)
list(APPEND UtilsDefinitions ${CommonCompileDefinitions} _CRT_SECURE_NO_WARNINGS)

include_directories(${SourceDir})
add_compile_options(${CommonCompileFlags})

add_library(GameLib SHARED ${SourceDir}game/fn_game.cpp)
set_target_properties(GameLib PROPERTIES OUTPUT_NAME game)
target_compile_options(GameLib PRIVATE -MTd -Fmgame.map)
target_compile_definitions(GameLib PRIVATE ${CommonCompileDefinitions})
target_link_options(GameLib PRIVATE ${CommonLinkerFlags})

add_executable(Game WIN32 ${SourceDir}platform/win32/win32_main.cpp)
set_target_properties(Game PROPERTIES OUTPUT_NAME Win64Game)
target_compile_options(Game PRIVATE -FmWin64Game.map)
target_compile_definitions(Game PRIVATE ${CommonCompileDefinitions})
target_link_libraries(Game PRIVATE ${CommonLinkerLibs})
target_link_options(Game PRIVATE ${CommonLinkerFlags})