cmake_minimum_required(VERSION 3.0)
project(streamer)

set(PLUGIN_VERSION "2.9.6")
set(CMAKE_CXX_STANDARD 17)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if (MSVC)
    add_definitions("/MP")
endif()

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/lib/cmake-modules")
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/lib/eigen/cmake")

function(GroupSourcesByFolder target)
	set(SOURCE_GROUP_DELIMITER "/")
	set(last_dir "")
	set(files "")

	get_target_property(sources ${target} SOURCES)

	foreach(file ${sources})
		file(RELATIVE_PATH relative_file "${CMAKE_CURRENT_SOURCE_DIR}" ${file})
		get_filename_component(dir "${relative_file}" PATH)
		if(NOT "${dir}" STREQUAL "${last_dir}")
			if(files)
				source_group("${last_dir}" FILES ${files})
			endif()
			set(files "")
		endif()
		set(files ${files} ${file})
		set(last_dir "${dir}")
	endforeach()

	if(files)
		source_group("${last_dir}" FILES ${files})
	endif()
endfunction()

set(SAMP_SDK_ROOT "${PROJECT_SOURCE_DIR}/lib/samp-plugin-sdk")
find_package(SAMPSDK REQUIRED)
include_directories(${SAMPSDK_DIR})
include_directories(${SAMPSDK_DIR}/amx)

include_directories(${PROJECT_SOURCE_DIR}/lib/boost)

if(EIGEN3_INCLUDE_DIR)
	file(TO_CMAKE_PATH ${EIGEN3_INCLUDE_DIR} EIGEN3_INCLUDE_DIR)
	list(APPEND CMAKE_MODULE_PATH "${EIGEN3_INCLUDE_DIR}/cmake")
else()
	set(EIGEN3_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/lib/eigen")
endif()

find_package(Eigen3 3.3.4 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIR})

set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY True)
set(CMAKE_SKIP_PACKAGE_ALL_DEPENDENCY True)

include(CheckTypeSize)
check_type_size("void*" SIZEOF_VOID_P BUILTIN_TYPES_ONLY)

if(NOT WIN32)
	if(NOT TARGET_ARCH OR TARGET_ARCH STREQUAL "x86")
		set(CMAKE_C_FLAGS "-m32 ${CMAKE_C_FLAGS}")
		set(CMAKE_CXX_FLAGS "-m32 ${CMAKE_CXX_FLAGS}")
	endif()
endif()

message("YO? ${TARGET_ARCH}")
if(TARGET_ARCH STREQUAL "x64")
	
	add_subdirectory(lib/omp-node)
endif()

add_subdirectory(lib/omp-sdk)
add_subdirectory(lib/omp-gdk)
include_directories(${OMP_GDK_PATH})
add_subdirectory(lib/omp-network)
add_subdirectory(src)

set(CPACK_PACKAGE_VERSION ${PLUGIN_VERSION})

if(COMMIT_ID)
	string(TIMESTAMP CURRENT_DATE "%Y%m%d")
	set(PACKAGE_FILE_NAME streamer-${PLUGIN_VERSION}-${CURRENT_DATE}-${COMMIT_ID})
else()
	set(PACKAGE_FILE_NAME streamer-${PLUGIN_VERSION})
endif()

set(CPACK_OUTPUT_FILE_PREFIX package/${PACKAGE_FILE_NAME})

if(WIN32)
	set(CPACK_PACKAGE_FILE_NAME ${PACKAGE_FILE_NAME}-win32)
	set(CPACK_GENERATOR ZIP)
else()
	set(CPACK_PACKAGE_FILE_NAME ${PACKAGE_FILE_NAME}-linux)
	set(CPACK_GENERATOR TGZ)
endif()

if(CPACK_OUTPUT_FILE_POSTFIX)
	string(TOLOWER "${CPACK_OUTPUT_FILE_POSTFIX}" CPACK_OUTPUT_FILE_POSTFIX)
	set(CPACK_PACKAGE_FILE_NAME ${CPACK_PACKAGE_FILE_NAME}-${CPACK_OUTPUT_FILE_POSTFIX})
endif()

include(CPack)
