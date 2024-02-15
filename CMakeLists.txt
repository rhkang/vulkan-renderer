cmake_minimum_required (VERSION 3.8)

project("Vulkan_Renderer")

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

find_package(Vulkan REQUIRED)

include(FetchContent)

FetchContent_Declare(
	VulkanMemoryAllocator
    GIT_REPOSITORY		"https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git"
    GIT_TAG				"v3.0.1"
    GIT_SHALLOW OFF
)
FetchContent_MakeAvailable(VulkanMemoryAllocator)

FetchContent_Declare(
	glfw
    GIT_REPOSITORY		"https://github.com/glfw/glfw"
    GIT_TAG				"3.3.9"
    GIT_SHALLOW ON
)
FetchContent_MakeAvailable(glfw)

FetchContent_Declare(
    fetch_vk_bootstrap
    GIT_REPOSITORY		https://github.com/charles-lunarg/vk-bootstrap
    GIT_TAG				"v1.3.277"
	GIT_SHALLOW ON
)
FetchContent_MakeAvailable(fetch_vk_bootstrap)

FetchContent_Declare(
	glm
	GIT_REPOSITORY		https://github.com/g-truc/glm.git
	GIT_TAG 			"1.0.0"
	GIT_SHALLOW ON
)
FetchContent_MakeAvailable(glm)

FetchContent_Declare(
	fmt
	GIT_REPOSITORY		https://github.com/fmtlib/fmt.git
	GIT_TAG 			"10.2.1"
	GIT_SHALLOW ON
)
FetchContent_MakeAvailable(fmt)

add_subdirectory(src)
