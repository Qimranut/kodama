cmake_minimum_required(VERSION 3.20)

project(Kodama)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(mysofa CONFIG REQUIRED)

add_subdirectory(core)
add_subdirectory(test)

if (TARGET mysofa::mysofa-shared)
	target_link_libraries(KodamaCore PRIVATE mysofa::mysofa-shared)
else()
	target_link_libraries(KodamaCore PRIVATE mysofa::mysofa-static)
endif()