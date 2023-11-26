cmake_minimum_required(VERSION 3.5)

project (TestGame LANGUAGES CXX)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(CMAKE_CXX_STANDARD 17)

set(ENET_DIR ${PROJECT_SOURCE_DIR}/vendor/enet)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)

add_executable(main 
	"src/main.cpp" 
	"src/SDL.cpp" 
	"src/Game.cpp" 
	"src/Scene.cpp"
	"src/Components.cpp" 
	"src/input/KeyboardHandler.cpp"
	"src/TextureRepo.cpp"
	"src/math/Geometry.cpp"
	"src/math/Math.cpp"
	"src/input/InputManager.cpp"
	"src/input/MouseHandler.cpp"
	"src/PlayerManager.cpp"
	"src/input/InputState.cpp")
target_link_libraries(main SDL2::SDL2 SDL2::SDL2main SDL2_image::SDL2_image SDL2_mixer::SDL2_mixer) # SDL2main runs SDL2main function which then calls this source main

add_executable(demo_client "src/network/demo_client.cpp" "src/network/buffer.cpp")
target_include_directories(demo_client PRIVATE ${ENET_DIR}/include)

add_executable(demo_server "src/network/demo_server.cpp" "src/network/buffer.cpp")
target_include_directories(demo_server PRIVATE ${ENET_DIR}/include)

if(WIN32)
	target_link_libraries(demo_client PRIVATE WS2_32 WINMM ${ENET_DIR}/enet64.lib)
	target_link_libraries(demo_server PRIVATE WS2_32 WINMM ${ENET_DIR}/enet64.lib)
else()
	target_link_libraries(demo_client PRIVATE /usr/local/lib/libenet.dylib) # fixme: add FindENet.cmake to automate this
	target_link_libraries(demo_server PRIVATE /usr/local/lib/libenet.dylib) # fixme: add FindENet.cmake to automate this
endif()