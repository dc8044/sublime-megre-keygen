get_filename_component(CUSTOM_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(${CUSTOM_NAME})

set(SOURCES rsakey.cpp code.cpp keyserialize.cpp)
set(HEADERS rsakey.h keyserialize.h)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/libtommath)
include_directories(${CMAKE_SOURCE_DIR}/libtomcrypt/src/headers)
add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES})

if (MSVC)
	set(BUILD_COMMAND "${CMAKE_SOURCE_DIR}/prebuild/build.bat")	
	add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD COMMAND ${BUILD_COMMAND})	
	target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/libtommath/tommath.lib)
	target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/libtomcrypt/tomcrypt.lib)
endif(MSVC)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD 
	COMMAND ${PROJECT_NAME}
	ARGS ${CMAKE_BINARY_DIR}
	COMMENT "RSA key pair generated")