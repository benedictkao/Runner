set (LIB_NAME ${LIB_PREFIX}common)

add_library(${LIB_NAME} INTERFACE)
target_include_directories(${LIB_NAME} INTERFACE "include")