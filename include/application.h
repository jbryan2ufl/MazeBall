#pragma once

// stl includes
#include <iostream>
#include <vector>
#include <string>
#include <limits>
#include <cmath>
#include <numbers>
#include <memory>
#include <iomanip>

// render includes
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>

#include <filesystem>

// imgui
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// font


// project files
#include "shader.h"
#include "camera.h"
#include "textmanager.h"
#include "grid.h"
#include "exampleObject.h"
#include "windowData.h"
#include "shaderManager.h"
#include "crosshair.h"
#include "icosahedron.h"
#include "mazeball.h"
#include "roomba.h"

// experimental optional windows no border/transparent
// #ifdef _WIN32
// 	#define GLFW_EXPOSE_NATIVE_WIN32
// 	#include <GLFW/glfw3native.h>
// 	#include <windows.h>
// #endif

class Application
{
public:

	ImGuiIO* m_ioptr{};

	GLFWwindow* m_window{};

	// Shader m_gridShader{};

	ShaderManager& m_shaderManager;

	// Grid m_grid{m_windowData->m_windowSize.x, m_windowData->m_windowSize.y, 25, 25};

	bool mouseDragging{false};
	double lastX{};
	double lastY{};

	glm::vec2 m_mousePos{};
	glm::vec3 m_mousePos3D{};

	bool vsync{true};

	void draw();

	void process_input();

	Camera m_camera{};

	glm::vec2 normalizePoint(double x, double y);
	void updateMousePos3D();

	std::shared_ptr<WindowData> m_windowData{std::make_shared<WindowData>()};
	TextManager m_fpsText{};
	TextManager m_worldText{};
	ExampleObject m_obj{};
	// Icosahedron m_soccerBall{};
	Roomba m_roomba{};
	
	MazeBall m_ball{};
	Crosshair m_crosshair{};

	bool firstMouse{true};
	bool mouseFocus{false};

	bool m_collision{false};
public:

	Application();

	void init();

	void process_framebuffer_size(int, int);

	void process_key(int, int, int, int);

	void process_mouse_button(int, int, int);

	void process_cursor_position(double, double);

	void process_scroll(double, double);

	void run();

	void close();

	double lastTime{};
	double currentTime{};
	double deltaTime{};
	double totalTime{};
	double frameTime{};
	double startTime{};

	void updateFrameTime();
};

static void framebuffer_size_callback(GLFWwindow*, int, int);

static void key_callback(GLFWwindow*, int, int, int, int);

static void mouse_button_callback(GLFWwindow*, int, int, int);

static void cursor_position_callback(GLFWwindow* , double, double);

static void scroll_callback(GLFWwindow*, double, double);
