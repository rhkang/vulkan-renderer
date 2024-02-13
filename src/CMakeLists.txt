FILE(GLOB SOURCES *.cpp *.h)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories (${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(${PROJECT_NAME} Vulkan::Vulkan vk-bootstrap::vk-bootstrap glfw glm::glm fmt::fmt-header-only)

set_target_properties(${PROJECT_NAME} PROPERTIES
	CXX_STANDARD 20
	CXX_EXTENSIONS OFF
	COMPILE_WARNING_AS_ERROR ON)
