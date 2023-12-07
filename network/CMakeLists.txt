add_library(network STATIC)
target_sources(network 
	PRIVATE 
		"src/Client.cpp"
		"src/Server.cpp"
		"src/Buffer.cpp"
		"src/Common.cpp"
)
target_include_directories(network 
	PUBLIC
		"include"
)
target_link_libraries(network
	PUBLIC
		vendor.enet
		logging
)

add_executable(demo_client "demo/demo_client.cpp")
target_link_libraries(demo_client PRIVATE network)

add_executable(demo_server "demo/demo_server.cpp")
target_link_libraries(demo_server PRIVATE network)