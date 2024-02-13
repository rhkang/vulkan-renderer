#include <EventHandler.h>

#include <iostream>
#include <fmt/core.h>

#include <map>
#include <queue>

std::map<int, std::function<void(void*)>> actions;
std::queue<int> KeyEventQueue{};

GLFWwindow* window{ nullptr };

void InitEventHandler(GLFWwindow* wnd) {
	window = wnd;

	// Register Key Events
	actions[GLFW_KEY_P] = [&](void* data) {
		exit(-1);
	};

	actions[GLFW_KEY_ESCAPE] = [&](void* data) {
		glfwSetWindowShouldClose(window, true);
	};
}

void EventHandle() {
	KeyEventHandle();
}

void KeyEventHandle() {
	int limit = 10;
	while (limit-- && !KeyEventQueue.empty()) {
		auto key = KeyEventQueue.front();
		KeyEventQueue.pop();

#ifdef KEY_EVENT_DEBUG
		fmt::print("Key Pressed event dequeued: {}\n", key);
#endif
		auto action = actions.find(key);
		if (action != actions.end()) {
			(action->second)(nullptr);
		}
	}
}

void KeyProcess(GLFWwindow* win, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_UNKNOWN) return; // Don't accept unknown keys

	if (action == GLFW_PRESS) {
		pressed[key] = true;
		KeyEventQueue.push(key);
	}
	else if (action == GLFW_RELEASE)
		pressed[key] = false;
}
