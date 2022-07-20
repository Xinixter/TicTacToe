#include "game.h"


static void CursorPosCallback(GLFWwindow* window, double xPos, double yPos);
static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
static void FrameBufferSizeCallback(GLFWwindow* window, int width, int height);
static void ProcessInput(GLFWwindow* window);

Game::Game() : m_Window { nullptr }
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	std::cout << "Initiated" << std::endl;

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
}

Game::~Game()
{
	glfwTerminate();
}

int Game::Init()
{
	m_Window = glfwCreateWindow(m_Width, m_Height, "TicTacToe", NULL, NULL);
	if (m_Window == NULL) {
		std::cerr << "Failed to create GLFW window" << std::endl;
		return -1;
	}

	glfwMakeContextCurrent(m_Window);
	glfwSetFramebufferSizeCallback(m_Window, FrameBufferSizeCallback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glfwSetInputMode(m_Window, GLFW_STICKY_KEYS, 1);
	glfwSetMouseButtonCallback(m_Window, MouseButtonCallback);

	const char* vertexShaderSource = R"GLSL(
		#version 330 core
		layout (location = 0) in vec3 posA;
		void main()
		{
			gl_Position = vec4(posA.x, posA.y, posA.z, 1.0);
		}
	)GLSL";

	unsigned int vertexShader {};
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	int  success {};
	char logInfo[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, logInfo);
		std::cout << "Error shader vertex compilation failed\n"
				  << logInfo << '\n';
	}

	const char* fragmentShaderSource = R"GLSL(
		#version 330 core
		out vec4 fragColor;
		void main()
		{
			fragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
		}
	)GLSL";

	unsigned int fragmentShader {};
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, logInfo);
		std::cout << "Error shader fragment compilation failed\n"
				  << logInfo << '\n';
	}

	unsigned int shaderProgram {};
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, logInfo);
		std::cout << "Error link failed\n" << logInfo << '\n';
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	float vertices[] = {
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 0.0f,  0.5f, 0.0f
	};

	unsigned int vao {};
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	unsigned int vbo {};
	glGenBuffers(1, &vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


	glVertexAttribPointer(
		0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);    // Unbinding buffer
	glBindVertexArray(0);
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Wirefram polygon

	while (!glfwWindowShouldClose(m_Window)) {
		ProcessInput(m_Window);

		glClearColor(0.102f, 0.102f, 0.102f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(m_Window);
		glfwPollEvents();
	}

	return 0;
}

static void ProcessInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

static void FrameBufferSizeCallback(
	GLFWwindow* window,
	const int   width,
	const int   height)
{
	glViewport(0, 0, width, height);
}

static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{

	if (double xPos {}, yPos {};
		button == GLFW_MOUSE_BUTTON_LEFT and action == GLFW_PRESS) {
		glfwGetCursorPos(window, &xPos, &yPos);
		std::cout << "Left mouse button at " << xPos << ',' << yPos
				  << std::endl;
	}
}
