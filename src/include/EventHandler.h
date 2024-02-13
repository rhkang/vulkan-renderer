#include <functional>

#include <GLFW/glfw3.h>

#define KEY_EVENT_DEBUG

const int KEYS = GLFW_KEY_MENU + 1;

static bool pressed[KEYS]{ false };

void InitEventHandler(GLFWwindow* window);

void EventHandle();

void KeyEventHandle();

void KeyProcess(GLFWwindow* win, int key, int scancode, int action, int mods);
