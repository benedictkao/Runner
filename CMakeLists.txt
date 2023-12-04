cmake_minimum_required(VERSION 3.5)

project (Runner LANGUAGES CXX)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(CMAKE_CXX_STANDARD 20)

## Network library, this can be put into separate file like FindNetwork.cmake

set(ENET_DIR "${PROJECT_SOURCE_DIR}/network/vendor/enet")

add_library(network STATIC)
target_sources(network PRIVATE "network/src/buffer.cpp" "network/src/network.cpp" )
target_include_directories(network 
	PUBLIC 
		"${ENET_DIR}/include"
		"${PROJECT_SOURCE_DIR}/network/include"
	PRIVATE 
		"${PROJECT_SOURCE_DIR}/network/src"
)
if(WIN32)
	target_link_libraries(network PRIVATE WS2_32 WINMM ${ENET_DIR}/enet64.lib)
else()
	target_link_libraries(network PRIVATE /usr/local/lib/libenet.dylib) # fixme: add FindENet.cmake to automate this
endif()

## Network demo executables

add_executable(demo_client "network/demo/demo_client.cpp")
target_link_libraries(demo_client PRIVATE network)

add_executable(demo_server "network/demo/demo_server.cpp")
target_link_libraries(demo_server PRIVATE network)

## Game executable

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)

add_executable(main 
	"game/src/main.cpp" 
	"game/src/SDL.cpp" 
	"game/src/Game.cpp" 
	"game/src/Scene.cpp"
	"game/src/Components.cpp" 
	"game/src/input/KeyboardHandler.cpp"
	"game/src/TextureRepo.cpp"
	"game/src/math/Geometry.cpp"
	"game/src/math/Math.cpp"
	"game/src/input/InputManager.cpp"
	"game/src/input/MouseHandler.cpp"
	"game/src/PlayerManager.cpp"
	"game/src/input/InputState.cpp")
target_link_libraries(main SDL2::SDL2 SDL2::SDL2main SDL2_image::SDL2_image SDL2_mixer::SDL2_mixer) # SDL2main runs SDL2main function which then calls this source main