#pragma once
#include "pch.h"

class UserInput {
public:
	static GLFWwindow* window;

	static bool buttonPressed;
	static int32_t key;
	static int32_t scancode;
	static int32_t action;
	static int32_t mods;

	static bool mouseButtonPressed;
	static uint32_t mouseButton;
	static uint32_t mouseAction;
	static uint32_t mouseMods;

	static float mouseX;
	static float mouseY;
	static float mouseDeltaX;
	static float mouseDeltaY;

	static void Reset();

	static int32_t GetKey(int32_t keyCode);
	static int32_t GetMouseButton(int32_t mouseButton);

	static void SetInputWindow(GLFWwindow* _window);
};