#include <VkInitializer.h>

#include <VkTypes.h>
#include <VkBootstrap.h>

void vkinit::InitializeVkGraphics(GLFWwindow* window, VkInstance* pInstance, VkSurfaceKHR* pSurface, VkPhysicalDevice* pGpuDevice, VkDevice* pDevice, VkQueue* pGraphicsQueue, uint32_t* pGraphicsQueueFamily, VkDebugUtilsMessengerEXT* pDebugMessenger) {
	// Skip the boilerplate on setting common vulkan objects
	// https://github.com/charles-lunarg/vk-bootstrap/?tab=readme-ov-file#basic-usage

	// create Instance
	vkb::InstanceBuilder builder;

	auto inst_ret = builder.set_app_name("Vulkan Renderer")
		.request_validation_layers()
		.use_default_debug_messenger()
		.require_api_version(1, 3, 0)
		.build();

	vkb::Instance vkb_inst = inst_ret.value();
	auto instance = vkb_inst.instance;
	auto debugMessenger = vkb_inst.debug_messenger;

	VkSurfaceKHR surface;

	// select GPU, create VKDevice
#if defined(_WIN32)
	glfwCreateWindowSurface(instance, window, nullptr, &surface);
#else
	fmt::print("Surface creation implemented for only Windows so far. Terminate the application.\n");
	glfwSetWindowShouldClose(window, true);
	return;
#endif

	* pSurface = surface;

	VkPhysicalDeviceVulkan13Features features13{	// 1.3 features
		.synchronization2 = true,
		.dynamicRendering = true,	// skip renderpasses, framebuffers
	};

	VkPhysicalDeviceVulkan12Features features12{	// 1.2 features
		.descriptorIndexing = true,		// bindless textures
		.bufferDeviceAddress = true,	// enable using GPU pointers without binding buffers
	};

	vkb::PhysicalDeviceSelector selector{ vkb_inst };
	vkb::PhysicalDevice phys_dev = selector.set_surface(surface)
		.set_minimum_version(1, 3)
		.set_required_features_13(features13)
		.set_required_features_12(features12)
		.set_surface(surface)
		.select()
		.value();

	auto gpuDevice = phys_dev.physical_device;

	vkb::DeviceBuilder device_builder{ phys_dev };
	vkb::Device vkb_device = device_builder.build().value();
	auto device = vkb_device.device;

	auto graphicsQueue = vkb_device.get_queue(vkb::QueueType::graphics).value();
	auto graphicsQueueFamily = vkb_device.get_queue_index(vkb::QueueType::graphics).value();

	*pInstance = instance;
	*pGpuDevice = gpuDevice;
	*pDevice = device;
	*pGraphicsQueue = graphicsQueue;
	*pGraphicsQueueFamily = graphicsQueueFamily;
	*pDebugMessenger = debugMessenger;
}

void vkinit::InitAllocator(VkInstance* pInstance, VkPhysicalDevice* pGpuDevice, VkDevice* pDevice, VmaAllocator* pAllocator) {
	VmaAllocatorCreateInfo vmaAllocatorCI{
		.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT,
		.physicalDevice = *pGpuDevice,
		.device = *pDevice,
		.instance = *pInstance,
	};

	VmaAllocator vmaAllocator;
	vmaCreateAllocator(&vmaAllocatorCI, &vmaAllocator);

	*pAllocator = vmaAllocator;
}

VkCommandPoolCreateInfo vkinit::commandPoolCI(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags)
{
	return VkCommandPoolCreateInfo{
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.flags = flags,
		.queueFamilyIndex = queueFamilyIndex,
	};
}

VkCommandBufferAllocateInfo vkinit::commandBufferAI(VkCommandPool commandPool, uint32_t count)
{
	return VkCommandBufferAllocateInfo{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.commandPool = commandPool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = count,
	};
}

VkFenceCreateInfo vkinit::fenceCI(VkFenceCreateFlags flags){
	return VkFenceCreateInfo{
		.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		.flags = flags,
	};
};

VkSemaphoreCreateInfo vkinit::semaphoreCI(VkSemaphoreCreateFlags flags) {
	return VkSemaphoreCreateInfo{
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
		.flags = flags,
	};
}

VkCommandBufferBeginInfo vkinit::commandBufferBI(VkCommandBufferUsageFlags flags) {
	return VkCommandBufferBeginInfo{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.flags = flags,
	};
}

VkImageSubresourceRange vkinit::ImageSubResourceRange(VkImageAspectFlags aspectMask) {
	return VkImageSubresourceRange{
		.aspectMask = aspectMask,
		.baseMipLevel = 0,
		.levelCount = VK_REMAINING_MIP_LEVELS,
		.baseArrayLayer = 0,
		.layerCount = VK_REMAINING_ARRAY_LAYERS,
	};
}

VkSemaphoreSubmitInfo vkinit::SemaphoreSI(VkPipelineStageFlags2 stageMask, VkSemaphore semaphore) {
	return VkSemaphoreSubmitInfo{
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
		.semaphore = semaphore,
		.value = 1,
		.stageMask = stageMask,
		.deviceIndex = 0,
	};
}

VkCommandBufferSubmitInfo vkinit::CommandBufferSI(VkCommandBuffer cmd) {
	return VkCommandBufferSubmitInfo{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
		.commandBuffer = cmd,
		.deviceMask = 0,
	};
}

VkSubmitInfo2 vkinit::SubmitInfo(VkCommandBufferSubmitInfo* cmdSI, VkSemaphoreSubmitInfo* signalSemaphoreSI, VkSemaphoreSubmitInfo* waitSemaphoreSI) {
	return VkSubmitInfo2{
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,

		.waitSemaphoreInfoCount = (uint32_t)(waitSemaphoreSI == nullptr ? 0 : 1),
		.pWaitSemaphoreInfos = waitSemaphoreSI,

		.commandBufferInfoCount = 1,
		.pCommandBufferInfos = cmdSI,

		.signalSemaphoreInfoCount = (uint32_t)(signalSemaphoreSI == nullptr ? 0 : 1),
		.pSignalSemaphoreInfos = signalSemaphoreSI,
	};
}

VkImageCreateInfo vkinit::ImageCI(VkFormat format, VkImageUsageFlags usageFlags, VkExtent3D extent) {
	return VkImageCreateInfo{
		.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		.imageType = VK_IMAGE_TYPE_2D,
		.format = format,
		.extent = extent,
		.mipLevels = 1,
		.arrayLayers = 1,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.tiling = VK_IMAGE_TILING_OPTIMAL,
		.usage = usageFlags,
	};
}

VkImageViewCreateInfo vkinit::ImageViewCI(VkFormat format, VkImage image, VkImageAspectFlags aspectFlags) {
	return VkImageViewCreateInfo{
		.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.image = image,
		.format = format,
		.subresourceRange {
			.aspectMask = aspectFlags,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1,
		},
	};
}
