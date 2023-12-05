find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)

add_executable(client 
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
	"src/input/InputState.cpp"
)
target_link_libraries(client
	SDL2::SDL2
	SDL2::SDL2main	# SDL2main runs SDL2main function which then calls this source main
	SDL2_image::SDL2_image
	SDL2_mixer::SDL2_mixer
	logging
	common
	network
)
#target_include_directories(main PRIVATE "${PROJECT_SOURCE_DIR}/game/src")