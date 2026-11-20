file(GLOB_RECURSE CORE_SOURCES CONFIGURE_DEPENDS
	"${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/*.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)

add_library(KodamaCore STATIC
	${CORE_SOURCES}
)

if (TARGET mysofa::mysofa-shared)
    target_link_libraries(KodamaCore PUBLIC mysofa::mysofa-shared)
else()
    target_link_libraries(KodamaCore PUBLIC mysofa::mysofa-static)
endif()

target_include_directories(KodamaCore
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/include
)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${CORE_SOURCES})