#include <vulkan/vulkan.h>

#include <GLFW/glfw3.h>

namespace vkinit {
	void InitializeVkGraphics(GLFWwindow* window, VkInstance* pInstance, VkSurfaceKHR* pSurface, VkPhysicalDevice* pGpuDevice, VkDevice* pDevice, VkQueue* pGraphicsQueue, uint32_t* pGraphicsQueueFamily, VkDebugUtilsMessengerEXT* pDebugMessenger);
	VkCommandPoolCreateInfo commandPoolCI(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags);
	VkCommandBufferAllocateInfo commandBufferAI(VkCommandPool commandPool, uint32_t count);
	VkFenceCreateInfo fenceCI(VkFenceCreateFlags flags = 0);
	VkSemaphoreCreateInfo semaphoreCI(VkSemaphoreCreateFlags flags = 0);
	VkCommandBufferBeginInfo commandBufferBI(VkCommandBufferUsageFlags flags);
	VkImageSubresourceRange ImageSubResourceRange(VkImageAspectFlags aspectMask);
	VkSemaphoreSubmitInfo SemaphoreSubmitInfo(VkPipelineStageFlags2 stageMask, VkSemaphore semaphore);
	VkCommandBufferSubmitInfo CommandBufferSubmitInfo(VkCommandBuffer cmd);
	VkSubmitInfo2 SubmitInfo(VkCommandBufferSubmitInfo* cmdSI, VkSemaphoreSubmitInfo* signalSemaphoreSI, VkSemaphoreSubmitInfo* waitSemaphoreSI);
}