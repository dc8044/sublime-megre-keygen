project(Generator)

set(SOURCES keygen.cpp code.cpp patch.cpp peparser.cpp disasm.cpp clip.cpp)
set(HEADERS keygen.h patch.h peparser.h disasm.h section.h clip.h
	graphics/resource.h graphics/graphicIds.h)

include_directories(${CMAKE_SOURCE_DIR}/pelib/include/)
include_directories(${CMAKE_SOURCE_DIR}/pelib/include/pelib)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR}/libtommath)
include_directories(${CMAKE_SOURCE_DIR}/libtomcrypt/src/headers)
include_directories(${CMAKE_SOURCE_DIR}/zydis/include)
include_directories(${CMAKE_SOURCE_DIR}/zydis/dependencies/zycore/include)
include_directories(${CMAKE_SOURCE_DIR}/zydis/msvc)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /MANIFEST:NO")
add_executable(${PROJECT_NAME} WIN32 
	${HEADERS} ${SOURCES} graphics/DIALOG.RC graphics/Generator.manifest)
target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/libtommath/tommath.lib)
target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/libtomcrypt/tomcrypt.lib)
target_link_libraries(${PROJECT_NAME} Comdlg32.lib)
target_link_libraries(${PROJECT_NAME} 
	${CMAKE_BINARY_DIR}/pelib/src/pelib/RelWithDebInfo/pelib.lib)
target_link_libraries(${PROJECT_NAME} "Zydis")

set(RSA_SUBPROJECT rsa_key_creator)
add_subdirectory(${RSA_SUBPROJECT})
add_dependencies(${PROJECT_NAME} ${RSA_SUBPROJECT})