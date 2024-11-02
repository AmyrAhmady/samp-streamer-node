include(AMXConfig)
include(AddSAMPPlugin)

set(PLUGIN_SOURCES ${SAMPSDK_DIR}/amxplugin.cpp)

add_subdirectory(manipulation)
add_subdirectory(pawnapi)
add_subdirectory(utility)
add_subdirectory(jsapi)

list(APPEND PLUGIN_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/cell.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/chunk-streamer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/core.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/data.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/grid.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/identifier.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/item.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/player.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/streamer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/omp-nethack.cpp
)

if(WIN32)
	list(APPEND PLUGIN_SOURCES
		${PROJECT_SOURCE_DIR}/streamer.rc
	)
endif()

# add_samp_plugin(${PROJECT_NAME} ${PLUGIN_SOURCES})

add_library(${PROJECT_NAME} MODULE ${PLUGIN_SOURCES})

set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")

if(CMAKE_COMPILER_IS_GNUCXX)
	set_property(TARGET ${PROJECT_NAME} APPEND_STRING PROPERTY
								COMPILE_FLAGS " -Wno-attributes")
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	set_property(TARGET ${PROJECT_NAME} APPEND_STRING PROPERTY
								COMPILE_FLAGS " -Wno-ignored-attributes")
endif()

if(WIN32 AND CMAKE_COMPILER_IS_GNUCC)
	set_property(TARGET ${PROJECT_NAME} APPEND_STRING PROPERTY
								LINK_FLAGS " -Wl,--enable-stdcall-fixup")
endif()

if(CYGWIN)
	set_property(TARGET ${PROJECT_NAME} APPEND PROPERTY COMPILE_DEFINITIONS "WIN32")
	set_property(TARGET ${PROJECT_NAME} APPEND_STRING PROPERTY LINK_FLAGS " -Wl,--kill-at")
elseif(UNIX AND NOT WIN32 AND NOT APPLE)
	set_property(TARGET ${PROJECT_NAME} APPEND PROPERTY COMPILE_DEFINITIONS "LINUX")
endif()

if(MINGW)
	# Work around missing #include <stddef.h> in <SDK>/amx/amx.h.
	set_property(TARGET ${PROJECT_NAME} APPEND_STRING PROPERTY COMPILE_FLAGS " -include stddef.h")
endif()

GroupSourcesByFolder(${PROJECT_NAME})

if(WIN32)
	add_definitions(-DNOMINMAX)
else()
	set_property(TARGET ${PROJECT_NAME} APPEND_STRING PROPERTY COMPILE_FLAGS " -fno-strict-aliasing")
	set_property(TARGET ${PROJECT_NAME} APPEND_STRING PROPERTY LINK_FLAGS " -lrt")
endif()

add_definitions(
	-D_SILENCE_CXX17_NEGATORS_DEPRECATION_WARNING 
	-D_SILENCE_CXX17_ADAPTOR_TYPEDEFS_DEPRECATION_WARNING 
	-D_SILENCE_CXX17_OLD_ALLOCATOR_MEMBERS_DEPRECATION_WARNING
	-D_SILENCE_CXX17_RESULT_OF_DEPRECATION_WARNING
)

set_target_properties(${PROJECT_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
target_link_libraries(${PROJECT_NAME} OMP-SDK omp-gdk OMP-Network)

target_link_libraries(${PROJECT_NAME} omp-node-sdk)
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
