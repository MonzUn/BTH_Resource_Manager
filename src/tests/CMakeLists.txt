cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(ResourceManagerTest)

file(GLOB SOURCES
	"${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

set(LIBRARIES
	SDL2
	SDL2main
	glew32
	glu32
	zdll
	zzip)

set(RELPATH "${CMAKE_CURRENT_SOURCE_DIR}/../")
get_filename_component(RELPATH ${RELPATH} ABSOLUTE)
foreach(f ${SOURCES})
    file(RELATIVE_PATH SRCGR ${RELPATH} ${f})
    string(REGEX REPLACE "(.*)(/[^/]*)$" "\\1" SRCGR ${SRCGR})
    string(REPLACE / \\ SRCGR ${SRCGR})
    source_group("${SRCGR}" FILES ${f})
endforeach()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/bin/debug/")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/bin/debug/")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/bin/release/")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/bin/release/")

include_directories(
	"${CMAKE_CURRENT_SOURCE_DIR}/../../externals/include"
	"${CMAKE_CURRENT_SOURCE_DIR}/../resourcemanager/interface"
)

if(${CMAKE_GENERATOR} STREQUAL "Visual Studio 14 2015 Win64")
	link_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../externals/lib/x64")
elseif(${CMAKE_GENERATOR} STREQUAL "Visual Studio 14 2015")
	link_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../externals/lib/x86")
endif()

add_executable(ResourceManagerTest ${SOURCES})

if(NOT TARGET ResourceManager)
	add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../resourcemanager" ResourceManager)
endif()

target_link_libraries(ResourceManagerTest
					  ${LIBRARIES}
					  ResourceManager)

if(${CMAKE_GENERATOR} STREQUAL "Visual Studio 14 2015 Win64")
	add_custom_command(TARGET ResourceManagerTest POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory
			"${CMAKE_CURRENT_SOURCE_DIR}/../../externals/lib/x64"
			$<TARGET_FILE_DIR:ResourceManagerTest>)
elseif(${CMAKE_GENERATOR} STREQUAL "Visual Studio 14 2015")
	add_custom_command(TARGET ResourceManagerTest POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory
			"${CMAKE_CURRENT_SOURCE_DIR}/../../externals/lib/x86"
			$<TARGET_FILE_DIR:ResourceManagerTest>)
endif()
