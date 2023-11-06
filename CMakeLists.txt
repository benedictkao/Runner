cmake_minimum_required(VERSION 3.5)

project (TestGame LANGUAGES CXX)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(CMAKE_CXX_STANDARD 17)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)

add_executable(main 
	"src/main.cpp" 
	"src/SDL.cpp" 
	"src/Game.cpp" 
	"src/Scene.cpp"
	"src/Components.cpp" 
	"src/PlayerState.cpp" 
	"src/input/KeyboardHandler.cpp"
	"src/TextureRepo.cpp"
	"src/math/Geometry.cpp"
	"src/math/Math.cpp"
	"src/input/InputManager.cpp"
	"src/input/MouseHandler.cpp")
target_link_libraries(main SDL2::SDL2 SDL2::SDL2main SDL2_image::SDL2_image SDL2_mixer::SDL2_mixer) # SDL2main runs SDL2main function which then calls this source main