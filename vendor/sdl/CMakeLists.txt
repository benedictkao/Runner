set (LIB_NAME ${LIB_PREFIX}.sdl)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)

add_library(${LIB_NAME} INTERFACE)
target_link_libraries(${LIB_NAME}
	INTERFACE
		SDL2::SDL2
		SDL2::SDL2main	# SDL2main runs SDL2main function which then calls this source main
		SDL2_image::SDL2_image
		SDL2_mixer::SDL2_mixer
)