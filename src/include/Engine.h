#if defined(_WIN32)
#define GLFW_INCLUDE_VULKAN
#endif

#include <deque>

#include <vulkan/vulkan.h>

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <EventHandler.h>
#include <Types.h>

struct DeletionQueue {
	std::deque<std::function<void()>> deletors;

	void Push(std::function<void()>&& function) {
		deletors.push_back(function);
	}

	void flush() {
		for (auto item = deletors.rbegin(); item != deletors.rend(); item++) {
			(*item)();
		}

		deletors.clear();
	}
};

struct FrameData {
	VkCommandPool commandPool;
	VkCommandBuffer commandBuffer;
	VkFence renderFence;
	VkSemaphore swapChainSemaphore;
	VkSemaphore renderSemaphore;
	DeletionQueue deletionQueue;
};

constexpr uint32_t NUM_FRAME = 2;

class Engine {
public:
	bool isInitialized{ false };
	int frameNumber{ 0 };
	bool stop_rendering{ false };
	VkExtent2D windowExtent{ 800, 600 };

	DeletionQueue mainDeletionQueue;

	GLFWwindow* window{ nullptr };

	static Engine& Get();

	void Init();
	void Run();
	void Cleanup();
	void Draw();

	VkInstance instance;
	VkDebugUtilsMessengerEXT debugMessenger;
	VkPhysicalDevice gpuDevice;
	VkDevice device;
	VkSurfaceKHR surface;
	VkSwapchainKHR swapChain;
	VkFormat swapChainImageFormat;

	std::vector<VkImage> swapChainImages;
	std::vector<VkImageView> swapChainImageViews;
	VkExtent2D swapChainExtent;

	FrameData frames[NUM_FRAME];
	FrameData& GetCurrentFrame() { return frames[frameNumber % NUM_FRAME]; }

	VkQueue graphicsQueue;
	uint32_t graphicsQueueFamily;

private:
	void InitVulkan();
	void InitSwapChain();
	void InitCommands();
	void InitSyncStructures();
	void DestroySwapChain();
};