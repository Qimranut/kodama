# Renders a short sound in a small room with a real HRTF dataset.
#   KodamaRoomDemo <hrir.sofa>
add_executable(KodamaRoomDemo
	room_demo.cpp
)

target_link_libraries(KodamaRoomDemo
	PRIVATE
		KodamaCore
)

target_compile_definitions(KodamaRoomDemo
	PRIVATE
		KODAMA_OUTPUT_DIR="${CMAKE_SOURCE_DIR}/output"
)