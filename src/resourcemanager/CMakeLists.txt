file(GLOB_RECURSE ResourceManagerSources
	"*.h"
	"*.cpp"
)

set(RESOURCEMANAGER_LIBRARIES
	SDL2
	SDL2main
	glew32
	glu32
	opengl32
	zdll
	zzip)

# Generate source groups depending on folders (creates Visual Studio filters)
set(RELPATH ${ProjectRoot}/src/resourcemanager)
get_filename_component(RELPATH ${RELPATH} ABSOLUTE)
foreach(f ${ResourceManagerSources})
    # Get the path of the file relative to ${DIRECTORY},
    # then alter it (not compulsory)
    file(RELATIVE_PATH SRCGR ${RELPATH} ${f})
    # Extract the folder, ie remove the filename part
    string(REGEX REPLACE "(.*)(/[^/]*)$" "\\1" SRCGR ${SRCGR})

    # Source_group expects \\ (double antislash), not / (slash)
    string(REPLACE / \\ SRCGR ${SRCGR})
    source_group("${SRCGR}" FILES ${f})
endforeach()

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../externals/include")
link_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../externals/lib/x64")

add_definitions(-DMEMORYALLOCATOR_DLL_EXPORT)
add_library(ResourceManager SHARED ${ResourceManagerSources})

target_link_libraries(ResourceManager
					  ${RESOURCEMANAGER_LIBRARIES})

install(
	TARGETS ResourceManager DESTINATION lib
	LIBRARY DESTINATION lib
	RUNTIME DESTINATION bin/${OperatingSystemNameLowerCase}/${TargetArchitecture})
