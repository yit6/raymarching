#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include <fstream>
#include <sstream>

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
	int x = 600;
	int y = 600;

	printf("Program name %s\n", argv[0]);
 
   if( argc == 3 ) {
      printf("The argument supplied is %s\n", argv[1]);
	  x = atof(argv[1]);
      printf("The argument supplied is %s\n", argv[2]);
	  y = atof(argv[2]);
   }
   else if( argc > 3 ) {
      printf("Too many arguments supplied.\n");
   }
   else {
      printf("Two argument expected.\n");
   }
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;
	/* Create a windowed mode window and its OpenGL context */
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	window = glfwCreateWindow(x, y, "Raymarching", NULL, NULL);
	glfwSwapInterval(1);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}
	float ratio = x/y;
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
	int locationr=glGetUniformLocation(shader,"ratio");
	float t=0;
	glUniform1f(location, 0.3);
	glUniform1f(locationr, ratio);
	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
		glUniform1f(location, t);
		t+=0.0003;
		//printf("%f\n",t);
		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
