add_executable(client 
	"src/main.cpp"

	"src/GameClient.cpp"

	"src/res/TextureRepo.cpp"

	"src/connection/ConnectionManager.cpp"
	
	"src/component/Components.cpp"

	"src/scene/Scene.cpp"
	"src/scene/SceneLoader.cpp"

	"src/player/PlayerManager.cpp"
	"src/player/PlayerRepo.cpp"

	"src/sdl/SDL.cpp"

	"src/math/Math.cpp"
	
	"src/input/KeyboardHandler.cpp"
	"src/input/InputManager.cpp"
	"src/input/MouseHandler.cpp"
	"src/input/InputState.cpp"
)
target_link_libraries(client
	PRIVATE
		vendor.entt
		vendor.sdl

		lib.logging
		lib.network
		
		game.common
)
target_include_directories(client PUBLIC "src")