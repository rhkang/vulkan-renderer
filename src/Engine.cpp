#include <Engine.h>

#include <chrono>
#include <thread>
#include <cassert>

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

#include <VkBootstrap.h>
#include <VkInitializer.h>
#include <VkImages.h>

constexpr bool bUseValidationLayers = false;

Engine* loadedEngine = nullptr;

Engine& Engine::Get() {
	return *loadedEngine;
}

void Engine::Init() {
	assert(loadedEngine == nullptr);
	loadedEngine = this;

	// Window Init
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	window = glfwCreateWindow(windowExtent.width, windowExtent.height, "Vulkan Render Engine", nullptr, nullptr);

	InitEventHandler(window);
	glfwSetKeyCallback(window, KeyProcess);

	// Vulkan Init
	InitVulkan();
	InitSwapChain();
	InitCommands();
	InitSyncStructures();

	isInitialized = true;
}

void Engine::Run() {
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		EventHandle();

		if (stop_rendering) {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			continue;
		}

		Draw();
	}
}

void Engine::Cleanup() {
	if (isInitialized) {
		vkDeviceWaitIdle(device);
		mainDeletionQueue.flush();

		for (int i = 0; i < NUM_FRAME; i++) {
			vkDestroyCommandPool(device, frames[i].commandPool, nullptr);
			vkDestroyFence(device, frames[i].renderFence, nullptr);
			vkDestroySemaphore(device, frames[i].swapChainSemaphore, nullptr);
			vkDestroySemaphore(device, frames[i].renderSemaphore, nullptr);
		}

		DestroySwapChain();

		vkDestroySurfaceKHR(instance, surface, nullptr);
		vkDestroyDevice(device, nullptr);

		vkb::destroy_debug_utils_messenger(instance, debugMessenger);
		vkDestroyInstance(instance, nullptr);

		glfwDestroyWindow(window);
		glfwTerminate();
	}

	loadedEngine = nullptr;
}

void Engine::Draw() {
	auto currentFrame = GetCurrentFrame();

	// let CPU wait for the end of GPU works
	VK_CHECK(vkWaitForFences(device, 1, &currentFrame.renderFence, true, ONESECOND_IN_NANO));
	currentFrame.deletionQueue.flush();
	VK_CHECK(vkResetFences(device, 1, &currentFrame.renderFence));

	uint32_t swapChainImageIndex;
	VK_CHECK(vkAcquireNextImageKHR(device, swapChain, ONESECOND_IN_NANO, currentFrame.swapChainSemaphore, nullptr, &swapChainImageIndex));

	VkCommandBuffer cmd = GetCurrentFrame().commandBuffer;
	VK_CHECK(vkResetCommandBuffer(cmd, 0));

	VkCommandBufferBeginInfo cmdBeginInfo = vkinit::commandBufferBI(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
	VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo));

	vkutil::TransitionImage(cmd, swapChainImages[swapChainImageIndex], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

	VkClearColorValue clearValue;
	float flash = abs(sin(frameNumber / 120.f));
	clearValue = { {0.0f, 0.0f, flash, 1.0f} };

	VkImageSubresourceRange clearRange = vkinit::ImageSubResourceRange(VK_IMAGE_ASPECT_COLOR_BIT);

	vkCmdClearColorImage(cmd, swapChainImages[swapChainImageIndex], VK_IMAGE_LAYOUT_GENERAL, &clearValue, 1, &clearRange);

	vkutil::TransitionImage(cmd, swapChainImages[swapChainImageIndex], VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

	VK_CHECK(vkEndCommandBuffer(cmd));

	VkCommandBufferSubmitInfo cmdInfo = vkinit::CommandBufferSI(cmd);
	VkSemaphoreSubmitInfo waitInfo = vkinit::SemaphoreSI(VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, currentFrame.swapChainSemaphore);
	VkSemaphoreSubmitInfo signalInfo = vkinit::SemaphoreSI(VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT, currentFrame.renderSemaphore);

	VkSubmitInfo2 submitInfo = vkinit::SubmitInfo(&cmdInfo, &signalInfo, &waitInfo);

	VK_CHECK(vkQueueSubmit2(graphicsQueue, 1, &submitInfo, currentFrame.renderFence));

	VkPresentInfoKHR presentInfo{
		.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &currentFrame.renderSemaphore,
		.swapchainCount = 1,
		.pSwapchains = &swapChain,
		.pImageIndices = &swapChainImageIndex,
	};

	VK_CHECK(vkQueuePresentKHR(graphicsQueue, &presentInfo));

	frameNumber++;
}

void Engine::InitVulkan()
{
	vkinit::InitializeVkGraphics(
		window, 
		&instance, 
		&surface, 
		&gpuDevice, 
		&device, 
		&graphicsQueue, 
		&graphicsQueueFamily,
		&debugMessenger
	);

	vkinit::InitAllocator(&instance, &gpuDevice, &device, &vmaAllocator);
}

void Engine::InitSwapChain() {
	vkb::SwapchainBuilder swapchainBuilder{ gpuDevice, device, surface };
	swapChainImageFormat = VK_FORMAT_B8G8R8A8_UNORM;

	auto vkbSwapChain = swapchainBuilder
		.set_desired_format(VkSurfaceFormatKHR{ .format = swapChainImageFormat, .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR })
		.set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
		.set_desired_extent(windowExtent.width, windowExtent.height)
		.add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
		.build()
		.value();

	swapChain = vkbSwapChain.swapchain;
	swapChainImages = vkbSwapChain.get_images().value();
	swapChainImageViews = vkbSwapChain.get_image_views().value();
	swapChainExtent = vkbSwapChain.extent;

	drawImage.imageFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
	drawImage.imageExtent = {
		.width = windowExtent.width,
		.height = windowExtent.height,
		.depth = 1,
	};

	VkImageUsageFlags drawImageUsages{
		VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
		VK_IMAGE_USAGE_TRANSFER_DST_BIT |
		VK_IMAGE_USAGE_STORAGE_BIT |
		VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
	};

	VkImageCreateInfo imageCI = vkinit::ImageCI(drawImage.imageFormat, drawImageUsages, drawImage.imageExtent);

	VmaAllocationCreateInfo allocCI{
		.usage = VMA_MEMORY_USAGE_GPU_ONLY,
		.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
	};

	vmaCreateImage(vmaAllocator, &imageCI, &allocCI, &drawImage.image, &drawImage.allocation, nullptr);

	VkImageViewCreateInfo imageViewCI = vkinit::ImageViewCI(drawImage.imageFormat, drawImage.image, VK_IMAGE_ASPECT_COLOR_BIT);
	VK_CHECK(vkCreateImageView(device, &imageViewCI, nullptr, &drawImage.imageView));

	mainDeletionQueue.push([=]() {
		vkDestroyImageView(device, drawImage.imageView, nullptr);
		vmaDestroyImage(vmaAllocator, drawImage.image, drawImage.allocation);
		});
}

void Engine::DestroySwapChain() {
	vkDestroySwapchainKHR(device, swapChain, nullptr);		// delete swapChainImages internally

	for (int i = 0; i < swapChainImageViews.size(); i++) {
		vkDestroyImageView(device, swapChainImageViews[i], nullptr);
	}
}

void Engine::InitCommands() {
	auto commandPoolCI = vkinit::commandPoolCI(graphicsQueueFamily, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
	
	for (int i = 0; i < NUM_FRAME; i++) {
		VK_CHECK(vkCreateCommandPool(device, &commandPoolCI, nullptr, &frames[i].commandPool));

		auto commandBufferAI = vkinit::commandBufferAI(frames[i].commandPool, 1);
		VK_CHECK(vkAllocateCommandBuffers(device, &commandBufferAI, &frames[i].commandBuffer));
	}
}

void Engine::InitSyncStructures() {
	VkFenceCreateInfo fenceCI = vkinit::fenceCI(VK_FENCE_CREATE_SIGNALED_BIT);
	VkSemaphoreCreateInfo semaphoreCI = vkinit::semaphoreCI();

	for (int i = 0; i < NUM_FRAME; i++) {
		VK_CHECK(vkCreateFence(device, &fenceCI, nullptr, &frames[i].renderFence));

		VK_CHECK(vkCreateSemaphore(device, &semaphoreCI, nullptr, &frames[i].swapChainSemaphore));
		VK_CHECK(vkCreateSemaphore(device, &semaphoreCI, nullptr, &frames[i].renderSemaphore));
	}
}
