#define STB_TRUETYPE_IMPLEMENTATION

#include "application.h"

void Application::run()
{
	while (!glfwWindowShouldClose(m_window))
	{
		currentTime = glfwGetTime();
		deltaTime = currentTime - lastTime;
		totalTime += deltaTime;

		process_input();
		draw();
		glfwPollEvents();

		lastTime = currentTime;
	}
}

void Application::updateFrameTime()
{
	double elapsedTime{currentTime-startTime};
	if (elapsedTime > 0.1)
	{
		frameTime = deltaTime;
		startTime=currentTime;
	}
}

void Application::draw()
{
	glClearColor(0.1,0.1,0.1,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_windowData->m_view = m_camera.getViewMatrix();

	// m_gridShader.use();
	// m_grid.draw();

	updateFrameTime();

	std::string frameTimeString{"Frame Time: "+std::to_string(frameTime*1000).substr(0,5)+"ms\n"};
	std::string fpsString{"FPS: "+std::to_string(static_cast<int>(1.0/frameTime))+'\n'};

	std::string mousePosString{"X:"+std::to_string(m_mousePos.x)+" Y:"+std::to_string(m_mousePos.y)};

	
	// OBJECT RENDERING
	m_obj.render();
	m_crosshair.render();

	m_collision = m_worldText.checkIntersection(m_camera.m_ray);

	// TEXT RENDERING
	m_worldText.render();

	m_fpsText.generateText(frameTimeString+fpsString+mousePosString);
	m_fpsText.render();

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGui::SetNextWindowBgAlpha(1.0f);
	static int width, height;
	glfwGetWindowSize(m_window, &width, &height);
	ImGui::SetNextWindowPos(ImVec2{width*m_windowData->m_viewRatio,0});
	ImGui::SetNextWindowSize(ImVec2{width*(1-m_windowData->m_viewRatio),height});
	if (ImGui::Begin("Settings", 0, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration))
	{
		if (ImGui::Checkbox("VSync", &vsync))
		{
			glfwSwapInterval(vsync);
		}
		if (ImGui::DragFloat3("Text Position", &m_worldText.m_modelMatrix.m_position[0], 1.0f/10));
		{
			m_worldText.m_modelMatrix.updateAll();
		}
		if (ImGui::DragFloat3("Text Rotation Axis", &m_worldText.m_modelMatrix.m_axis[0], 1.0f/360));
		{
			m_worldText.m_modelMatrix.updateAll();
		}
		if (ImGui::DragFloat("Text Rotation Angle", &m_worldText.m_modelMatrix.m_angle, 1.0f/360))
		{
			m_worldText.m_modelMatrix.updateAll();
		}
		if (ImGui::DragFloat("Text Scale", &m_worldText.m_modelMatrix.m_scaleFactor, 1.0f/360))
		{
			m_worldText.m_modelMatrix.updateAll();
		}

		
		bool m_nearPlane{ImGui::DragFloat("Near Plane", &m_camera.m_nearPlane, 0.0001f)};
		bool m_farPlane{ImGui::DragFloat("Far Plane", &m_camera.m_farPlane, 100.0f)};
		bool m_fov{ImGui::DragFloat("FOV", &m_camera.m_fov, 1.0f)};
		bool m_pos{ImGui::DragFloat3("Camera Position", &m_camera.m_position[0])};

		if (m_nearPlane || m_farPlane || m_fov || m_pos)
		{
			m_camera.updateCameraVectors();
			m_windowData->m_perspective = glm::perspective(glm::radians(m_camera.m_fov), m_windowData->m_aspectRatio, m_camera.m_nearPlane, m_camera.m_farPlane);
		}

		static glm::vec3 vec{};
		vec = m_worldText.m_boundingBox.m_min;
		ImGui::Text("MIN: %f, %f, %f", vec.x, vec.y, vec.z);
		vec = m_worldText.m_boundingBox.m_max;
		ImGui::Text("MAX: %f, %f, %f", vec.x, vec.y, vec.z);
		vec = m_camera.m_ray.origin;
		ImGui::Text("ORIGIN: %f, %f, %f", vec.x, vec.y, vec.z);
		vec = m_camera.m_ray.direction;
		ImGui::Text("DIRECTION: %f, %f, %f", vec.x, vec.y, vec.z);

		if (m_collision)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255)); // Green color
			ImGui::Text("YES");
		}
		else
		{
			ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255)); // Red color
			ImGui::Text("NO");
		}
		ImGui::PopStyleColor();
	}
	ImGui::End();
	ImGui::Render();

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	glfwSwapBuffers(m_window);
}

Application::Application()
	: m_shaderManager{ShaderManager::getInstance()}
{
}

void Application::init()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// glfwWindowHint(GLFW_SAMPLES, 8);

	m_window = glfwCreateWindow(m_windowData->m_windowSize.x, m_windowData->m_windowSize.y, "3D Template", NULL, NULL);

	if (m_window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		close();
	}

	glfwMakeContextCurrent(m_window);
	glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);
	glfwSetCursorPosCallback(m_window, cursor_position_callback);
	glfwSetScrollCallback(m_window, scroll_callback);
	glfwSetKeyCallback(m_window, key_callback);
	glfwSetMouseButtonCallback(m_window, mouse_button_callback);
	glfwSetScrollCallback(m_window, scroll_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize Glad" << std::endl;
		close();
	}

	glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	glEnable(GL_DEPTH_TEST);
	glfwSwapInterval(vsync);

	// glEnable(GL_CULL_FACE);
	// glCullFace(GL_BACK);

	glDisable(GL_CULL_FACE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_DEPTH_CLAMP);

	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		std::cerr << "Error already: " << error << std::endl;
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	m_ioptr=&io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(m_window, true);
	ImGui_ImplOpenGL3_Init();
	
	glfwSetWindowUserPointer(m_window, this);

	glViewport(0, 0, m_windowData->m_windowSize.x*m_windowData->m_viewRatio, m_windowData->m_windowSize.y);

	// experimental optional windows no border/transparent
	// #ifdef _WIN32
	// 	HWND hwnd = (HWND)glfwGetWin32Window(m_window);
	// 	LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);
	// 	style &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
	// 	SetWindowLongPtr(hwnd, GWL_STYLE, style | WS_EX_LAYERED);

	// 	style = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
	// 	SetWindowLongPtr(hwnd, GWL_EXSTYLE, style | WS_EX_LAYERED);
	// 	SetLayeredWindowAttributes(hwnd, RGB(255, 0, 0), 128, LWA_COLORKEY);
	// #endif

	m_windowData->m_view = m_camera.getViewMatrix();
	m_windowData->m_perspective = glm::perspective(glm::radians(m_camera.m_fov), m_windowData->m_aspectRatio, m_camera.m_nearPlane, m_camera.m_farPlane);
	m_windowData->m_orthographic = glm::ortho(0.0f, static_cast<float>(m_windowData->m_viewSize.x), static_cast<float>(m_windowData->m_viewSize.y), 0.0f, m_camera.m_nearPlane, m_camera.m_farPlane);

	// m_gridShader = Shader("../src/shaders/grid.vs", "../src/shaders/source.fs");
	// m_grid.init();
	// m_grid.populate();

	{
		auto shader{m_shaderManager.loadNewShader("Text", "../src/shaders/text.vs", "../src/shaders/text.fs")};
		m_fpsText.setShader(shader);
		m_worldText.setShader(shader);
		
		shader = m_shaderManager.loadNewShader("Object", "../src/shaders/object.vs", "../src/shaders/object.fs");
		m_obj.setShader(shader);
		m_crosshair.setShader(shader);
	}

	m_fpsText.initFont("../fonts/slkscr.ttf", 32.0f);
	m_fpsText.initTextProperties(glm::vec3{0.0f, 0.0f, 0.0f}, TextViewingMode::Orthographic, TextAnchor::TopLeft, TextJustification::Left);
	m_fpsText.init(m_windowData);

	m_worldText.initFont("../fonts/Arial.ttf", 96.0f);
	m_worldText.initTextProperties(glm::vec3{0.0f, 0.0f, 0.0f}, TextViewingMode::Perspective, TextAnchor::BottomLeft, TextJustification::Left);
	m_worldText.init(m_windowData);
	m_worldText.generateText("HELLO!");

	m_obj.init(m_windowData);
	m_crosshair.init(m_windowData);
}


void Application::process_framebuffer_size(int width, int height)
{
	glViewport(0, 0, width*m_windowData->m_viewRatio, height);
}

// FOR SINGLE PRESSES
void Application::process_key(int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_ESCAPE)
		{
			glfwSetWindowShouldClose(m_window, true);
			close();
		}
		if (key == GLFW_KEY_LEFT)
		{
			m_camera.m_yaw+=90.0f;
			m_camera.updateCameraVectors();
		}
	}
}

void Application::updateMousePos3D()
{
	m_mousePos = normalizePoint(lastX, lastY);

	glm::vec4 rayClip{glm::vec4{m_mousePos.x, m_mousePos.y, -1.0f, 1.0f}};
	glm::vec4 rayEye{glm::inverse(m_windowData->m_perspective) * rayClip};
	rayEye = glm::vec4{rayEye.x, rayEye.y, -1.0f, 0.0f};

	glm::vec3 rayWorld{glm::vec3{glm::inverse(m_windowData->m_view) * rayEye}};
	rayWorld = glm::normalize(rayWorld);

	m_mousePos3D = rayWorld;
}

glm::vec2 Application::normalizePoint(double x, double y)
{
	x=x/m_windowData->m_viewSize.x*2-1;
	y=y/m_windowData->m_viewSize.y*2-1;

	return glm::vec2{x, -y};
}

void Application::process_mouse_button(int button, int action, int mods)
{
	if (!m_ioptr->WantCaptureMouse)
	{
		if (button == GLFW_MOUSE_BUTTON_LEFT)
		{
			if (action == GLFW_PRESS)
			{
				mouseFocus = true;
				firstMouse = true;
				glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			}
			else if (action == GLFW_RELEASE)
			{
				mouseFocus = false;
				glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			}
		}
	}
}

void Application::process_cursor_position(double xposIn, double yposIn)
{
	if (!m_ioptr->WantCaptureMouse)
	{
		if (mouseFocus)
		{
			float xpos=static_cast<float>(xposIn);
			float ypos=static_cast<float>(yposIn);

			if (firstMouse)
			{
				lastX=xpos;
				lastY=ypos;
				firstMouse=false;
			}
			float xoffset = xpos-lastX;
			float yoffset = lastY-ypos;

			lastX=xpos;
			lastY=ypos;

			m_camera.processMouseMovement(xoffset, yoffset);
		}
	}
	updateMousePos3D();
}

void Application::process_scroll(double xoffset, double yoffset)
{
	yoffset > 0 ? m_worldText.m_modelMatrix.m_scaleFactor *= 1.1f : m_worldText.m_modelMatrix.m_scaleFactor /= 1.1f;

	// updateMousePos3D();
	glm::mat4 translateToCenter = glm::translate(glm::mat4{1.0f}, glm::vec3{m_mousePos, 0.0f});

	glm::mat4 scale = glm::scale(glm::mat4{1.0}, glm::vec3{m_worldText.m_modelMatrix.m_scaleFactor});

	glm::mat4 translateBack = glm::translate(glm::mat4{1.0f}, -glm::vec3{m_mousePos, 0.0f});

	glm::mat4 finalScale{translateToCenter * scale * translateBack};

	m_worldText.m_modelMatrix.m_scaleFactor = finalScale[0][0];
	m_worldText.m_boundingBox.transform(m_worldText.m_modelMatrix.m_matrix);
	m_worldText.m_modelMatrix.updateAll();
}

void Application::process_input()
{
	if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS)
	{
		m_camera.processKeyboardMovement(FORWARD, deltaTime);
	}
	if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS)
	{
		m_camera.processKeyboardMovement(BACKWARD, deltaTime);
	}
	if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS)
	{
		m_camera.processKeyboardMovement(LEFT, deltaTime);
	}
	if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS)
	{
		m_camera.processKeyboardMovement(RIGHT, deltaTime);
	}
	if (glfwGetKey(m_window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		m_camera.processKeyboardMovement(UP, deltaTime);
	}
	if (glfwGetKey(m_window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		m_camera.processKeyboardMovement(DOWN, deltaTime);
	}
}

void Application::close()
{
	glfwTerminate();
	exit(0);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	Application* obj = (Application*)glfwGetWindowUserPointer(window);
	obj->process_framebuffer_size(width, height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Application* obj = (Application*)glfwGetWindowUserPointer(window);
	obj->process_key(key, scancode, action, mods);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	Application* obj = (Application*)glfwGetWindowUserPointer(window);
	obj->process_mouse_button(button, action, mods);
}

void cursor_position_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	Application* obj = (Application*)glfwGetWindowUserPointer(window);
	obj->process_cursor_position(xposIn, yposIn);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	Application* obj = (Application*)glfwGetWindowUserPointer(window);
	obj->process_scroll(xoffset, yoffset);
}
