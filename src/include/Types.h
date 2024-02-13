#pragma once

#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>
#include <fmt/core.h>

const uint64_t ONESECOND_IN_NANO = 1000000000;

#define VK_CHECK(x)																															\
{																																			\
	VkResult res = x;																														\
	if (res != VK_SUCCESS)																													\
	{																																		\
		fmt::print("Fatal : VkResult is {0} in {1} at line {2}\n", string_VkResult(res), __FILE__, __LINE__);								\
		assert(res == VK_SUCCESS);																											\
	}																																		\
}																																			\
