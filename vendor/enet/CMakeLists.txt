set (LIB_NAME ${LIB_PREFIX}.enet)

add_library(${LIB_NAME} INTERFACE)
target_include_directories(${LIB_NAME}
	INTERFACE
		"include"
)
if(WIN32)
	target_link_libraries(${LIB_NAME}
		INTERFACE
			WS2_32
			WINMM
			"${CMAKE_CURRENT_SOURCE_DIR}/lib/enet64.lib"
	)
else()
	target_link_libraries(${LIB_NAME}
		INTERFACE
			/usr/local/lib/libenet.dylib	# fixme: add FindENet.cmake to automate this
	) 
endif()
target_compile_definitions(${LIB_NAME} INTERFACE NOMINMAX)