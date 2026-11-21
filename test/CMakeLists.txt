add_executable(KodamaTest
	src/main.cpp
)

target_link_libraries(KodamaTest
	PRIVATE
		KodamaCore
)

target_compile_definitions(KodamaTest
	PRIVATE
		KODAMA_ASSET_DIR="${CMAKE_SOURCE_DIR}/assets"
		KODAMA_OUTPUT_DIR="${CMAKE_SOURCE_DIR}/output"
)

# Regression checks: no audio files or SOFA data needed.
# Run with `ctest` from the build directory, or run KodamaChecks directly.
add_executable(KodamaChecks
	src/checks.cpp
	src/odr_check.cpp
)

target_link_libraries(KodamaChecks
	PRIVATE
		KodamaCore
)

add_test(NAME KodamaChecks COMMAND KodamaChecks)
