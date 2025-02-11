
//Programa que simplemente crea un contexto de trabajo con glfw


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


// Gestion de teclado
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}


int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();														// inicializa glfw
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);					//Version del Core de Opengl a usar
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);					//Cersion del core the Opengl a usar
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);	//Indica que debe crear el opengl contexto CORE

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Basico VisualstudioGlfwGlad", NULL, NULL); // crea la ventana
	if (window == NULL)
	{
		std::cout << "Eror al crear la ventana" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);							// crea el contexto de trabajo dentro de la ventana

        printf("Hello\n");

	// Captura los punteros de opengl
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Error con Glad" << std::endl;
		return -1;
	}

	// Lazo principal.

	while (!glfwWindowShouldClose(window))
	{
		// Entrada de teclado

		processInput(window);						// procesamos los eventos de entrada

		// Dibujo de fondo de pantalla.
		// ------
		glClearColor(1.0f, .0f, .0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glfwSwapBuffers(window);					//los eventos de glfw, techlado raton....
		glfwPollEvents();							// los eventos de openGl.
	}

	glfwTerminate();
	return 0;
}
