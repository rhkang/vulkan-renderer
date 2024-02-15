#include <vulkan/vulkan.h>

#include <GLFW/glfw3.h>

namespace vkinit {
	void InitializeVkGraphics(GLFWwindow* window, VkInstance* pInstance, VkSurfaceKHR* pSurface, VkPhysicalDevice* pGpuDevice, VkDevice* pDevice, VkQueue* pGraphicsQueue, uint32_t* pGraphicsQueueFamily, VkDebugUtilsMessengerEXT* pDebugMessenger);
	void InitAllocator(VkInstance* pInstance, VkPhysicalDevice* pGpuDevice, VkDevice* pDevice, VmaAllocator* pAllocator);
	VkCommandPoolCreateInfo commandPoolCI(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags);
	VkCommandBufferAllocateInfo commandBufferAI(VkCommandPool commandPool, uint32_t count);
	VkFenceCreateInfo fenceCI(VkFenceCreateFlags flags = 0);
	VkSemaphoreCreateInfo semaphoreCI(VkSemaphoreCreateFlags flags = 0);
	VkCommandBufferBeginInfo commandBufferBI(VkCommandBufferUsageFlags flags);
	VkImageSubresourceRange ImageSubResourceRange(VkImageAspectFlags aspectMask);
	VkSemaphoreSubmitInfo SemaphoreSI(VkPipelineStageFlags2 stageMask, VkSemaphore semaphore);
	VkCommandBufferSubmitInfo CommandBufferSI(VkCommandBuffer cmd);
	VkSubmitInfo2 SubmitInfo(VkCommandBufferSubmitInfo* cmdSI, VkSemaphoreSubmitInfo* signalSemaphoreSI, VkSemaphoreSubmitInfo* waitSemaphoreSI);
	VkImageCreateInfo ImageCI(VkFormat format, VkImageUsageFlags usageFlags, VkExtent3D extent);
	VkImageViewCreateInfo ImageViewCI(VkFormat format, VkImage image, VkImageAspectFlags aspectFlags);
}