set (LIB_NAME ${LIB_PREFIX}common)

add_library(${LIB_NAME} STATIC)
target_sources(${LIB_NAME} 
	PRIVATE
		"src/Geometry.cpp"
)
target_include_directories(${LIB_NAME} PUBLIC "include")