#include "pch.h"
#include "UserInput.h"

GLFWwindow* UserInput::window = nullptr;

bool UserInput::buttonPressed = false;
int32_t UserInput::key = 0;
int32_t UserInput::scancode = 0;
int32_t UserInput::action = 0;
int32_t UserInput::mods = 0;

bool UserInput::mouseButtonPressed;
uint32_t UserInput::mouseButton;
uint32_t UserInput::mouseAction;
uint32_t UserInput::mouseMods;

double UserInput::mouseX = 0;
double UserInput::mouseY = 0;
double UserInput::mouseDeltaX = 0;
double UserInput::mouseDeltaY = 0;

double UserInput::mouseScrollX = 0;
double UserInput::mouseScrollY = 0;

void key_callback(GLFWwindow* window, int32_t key, int32_t scancode, int32_t action, int32_t mods) {
	UserInput::buttonPressed = true;
	UserInput::key = key;
	UserInput::scancode = scancode;
	UserInput::action = action;
	UserInput::mods = mods;
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
	UserInput::mouseDeltaX = xpos - UserInput::mouseX;
	UserInput::mouseDeltaY = ypos - UserInput::mouseY;
	UserInput::mouseX = xpos;
	UserInput::mouseY = ypos;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	UserInput::mouseButtonPressed = true;
	UserInput::mouseButton = button;
	UserInput::mouseAction = action;
	UserInput::mouseMods = mods;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	UserInput::mouseScrollX = xoffset;
	UserInput::mouseScrollY = yoffset;
}

void UserInput::Reset() {
	buttonPressed = false;
	mouseButtonPressed = false;
	mouseDeltaX = 0;
	mouseDeltaY = 0;
	mouseScrollX = 0;
	mouseScrollY = 0;
}

int32_t UserInput::GetKey(int32_t keyCode) {
	return glfwGetKey(window, keyCode);
}

int32_t UserInput::GetMouseButton(int32_t mouseButton) {
	return glfwGetMouseButton(window, mouseButton);
}

void UserInput::SetInputWindow(GLFWwindow* _window) {
	window = _window;
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);
}
