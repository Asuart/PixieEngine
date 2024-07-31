#include "pch.h"
#include "UserInput.h"

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

float UserInput::mouseX = 0;
float UserInput::mouseY = 0;
float UserInput::mouseDeltaX = 0;
float UserInput::mouseDeltaY = 0;

void UserInput::Reset() {
	UserInput::buttonPressed = false;
	UserInput::mouseButtonPressed = false;
	UserInput::mouseDeltaX = 0;
	UserInput::mouseDeltaY = 0;
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
}
