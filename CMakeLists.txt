cmake_minimum_required(VERSION 3.20.0)

project(TicTacToe)

add_subdirectory(dependencies/glfw)

file(GLOB SOURCES src/*.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME}
	PUBLIC dependencies/glfw/include/
)

target_link_directories(${PROJECT_NAME}
	PRIVATE dependencies/glfw/src/
)

target_link_libraries(${PROJECT_NAME}
	glfw
)
