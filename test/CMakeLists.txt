add_executable(KodamaTest
	src/main.cpp
)

target_link_libraries(KodamaTest
	PRIVATE
		KodamaCore
)