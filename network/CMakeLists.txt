set (LIB_NAME "lib.network")

add_library(${LIB_NAME} STATIC)
target_sources(${LIB_NAME} 
	PRIVATE
		"src/Buffer.cpp"
		"src/Common.cpp"
)
target_include_directories(${LIB_NAME} 
	PUBLIC
		"include"
)
target_link_libraries(${LIB_NAME}
	PUBLIC
		vendor.enet
		lib.logging
)

add_executable(demo_client "demo/demo_client.cpp")
target_link_libraries(demo_client 
	PRIVATE 
		${LIB_NAME}
)

add_executable(demo_server "demo/demo_server.cpp")
target_link_libraries(demo_server 
	PRIVATE 
		${LIB_NAME}
)