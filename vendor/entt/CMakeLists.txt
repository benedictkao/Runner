set(LIB_NAME ${LIB_PREFIX}entt)

add_library(${LIB_NAME} INTERFACE)
target_include_directories(${LIB_NAME}
	INTERFACE
		"include"
)