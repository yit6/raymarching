#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>
#include <sstream>
double dist = 10;
std::string readFile(const char* path) {
	std::fstream file(path);
	std::string line;
	std::stringstream fileContents;
	if (file.is_open()) {
		while (std::getline(file, line)) {
			fileContents << line << "\n";
		}
	}
	else {
		printf("FILELOADER ERROR: No file exists at path: %s\n", path);
	}

	file.close();
	return fileContents.str();
}

static unsigned int CompileShader( unsigned int type, const std::string& source) {
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*) alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "Failed" << std::endl;
		std::cout << message << std::endl;
		glDeleteShader(id);
		return 0;
	}
	return id;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	dist-=yoffset/4;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);


	return program;
}

int main(int argc, char *argv[])
{

	GLFWwindow* window;
double xpos, ypos;
	/* Initialize the library */
	if (!glfwInit())
		return -1;
	/* Create a windowed mode window and its OpenGL context */
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	window = glfwCreateWindow(600, 600, "Raymarching", NULL, NULL);
	glfwSwapInterval(1);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}
	
	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	glewInit();

	float positions[] = {
		-1, -1,
	 	 1, -1,
		-1,  1,
		 1,  1
	};

	unsigned int indices[] = {
		0, 1, 2,
		2, 3, 1
	};

	unsigned int buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float)*2, 0);
	glEnableVertexAttribArray(0);

	unsigned int ibo;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	/*std::string fragmentShader = 
		"#version 330 core\n"
		"\n"
		"layout(location = 0) out vec4 color;\n"
		"\n"
		"void main() {\n"
		"color = vec4(1.0,0.0,0.0,0.5);\n"
		"}\n"
	;*/
	std::string fragmentShader = readFile("fragment.shader");
	std::string vertexShader = readFile("vertex.shader");
	
	/*std::string vertexShader = 
		"#version 330 core\n"
		"\n"
		"layout(location = 0) in vec4 position;\n"
		"\n"
		"void main() {\n"
		"gl_Position = position;\n"
		"}\n"
	;*/
	
	unsigned int shader = CreateShader(vertexShader, fragmentShader);
	glUseProgram(shader);
	int location=glGetUniformLocation(shader,"t");
	int locationw=glGetUniformLocation(shader,"width");
	int locationh=glGetUniformLocation(shader,"height");
	int locationx=glGetUniformLocation(shader,"mouseX");
	int locationy=glGetUniformLocation(shader,"mouseY");
	int locationd=glGetUniformLocation(shader,"camdist");
	glfwSetScrollCallback(window, scroll_callback);
	float t=0;
	glUniform1f(location, t);
	/* Loop until the user closes the window */



int display_h,display_w;
	while (!glfwWindowShouldClose(window))
	{
		glfwGetCursorPos(window, &xpos, &ypos);
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
		glUniform1f(location, t);
		t+=0.0003;
		//printf("%f\n",t);
		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
	
		glUniform1f(locationw, display_w);
		glUniform1f(locationh, display_h);
		glUniform1f(locationy, ypos);
		glUniform1f(locationx, xpos);
		glUniform1f(locationd, dist);
	}

	glfwTerminate();
	return 0;
}
