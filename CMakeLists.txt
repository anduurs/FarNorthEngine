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

list(APPEND CommonCompilerFlags -nologo -Z7 -MTd -fp:fast -FC -GR- -EHa- -Oi -Od -WX -W4 -wd4100 -wd4189)
list(APPEND CommonCompilerDefinitions -DDEBUG_BUILD=1 -DPLATFORM_WIN32=1)
list(APPEND CommonLinkerFlags -incremental:no -opt:ref)
list(APPEND CommonLinkerLibs user32.lib gdi32.lib)

include_directories(${SourceDir})
add_compile_options(${CommonCompilerFlags})

add_library(GameLib SHARED ${SourceDir}game/fn_game.cpp)
set_target_properties(GameLib PROPERTIES OUTPUT_NAME game)
target_compile_options(GameLib PRIVATE -Fmgame.map)
target_compile_definitions(GameLib PRIVATE ${CommonCompilerDefinitions})
target_link_options(GameLib PRIVATE ${CommonLinkerFlags})
add_custom_command(TARGET GameLib WORKING_DIRECTORY ${BuildDir}
	PRE_BUILD 
		COMMAND set ARGS LastError=%ERRORLEVEL%
		COMMAND ${CMAKE_COMMAND} -E make_directory ${BuildDir}
		COMMAND ${CMAKE_COMMAND} -E make_directory ${DataDir}
		COMMAND del ARGS game_*.pdb 
)
add_custom_command(TARGET GameLib WORKING_DIRECTORY ${BuildDir}
	POST_BUILD 
		COMMAND ren ARGS game.pdb game_%random%.pdb 
		COMMAND del ARGS lock.tmp
)

add_executable(Game WIN32 ${SourceDir}platform/win32/win32_main.cpp)
set_target_properties(Game PROPERTIES OUTPUT_NAME Win64Game)
target_compile_options(Game PRIVATE -FmWin64Game.map)
target_compile_definitions(Game PRIVATE ${CommonCompileDerfinitions})
target_link_libraries(Game PRIVATE ${CommonLinkerLibs})
target_link_options(Game PRIVATE ${CommonLinkerFlags})