
//Creamos unos ejes coordenados en opengl 3.3
//incluyo la lectura de shaders. Debe incluirse el fichero de lectura proporcionado.

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "lecturaShader_0_9.h"

#include <iostream>


////Funcion que genera el Programa Sader
GLuint setShaders(const char *nVertx, const char *nFrag);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


int shaderProgram; //defino esta variable como globla por comodidad
unsigned int VAO,VBO, EBO; // las uso globales por comodidad. Son los arrays que se utilizarán para dibujar los objetos.

//gestion de entrada
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}




void medianteArray() {
	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		0.0f, 0.0f, -0.5f, // x
		.5f, 0.0f, -0.5f,
		0.0f, 0.0f, -0.5, // y
		0.0f, .5f,-0.5f,
		0.0f,  0.0f, 0.0f,  // z
		0.0f, 0.0f, .5f,
	};

	// GENERO EL VAO y le hago el bind para que este activo
	glGenVertexArrays(1, &VAO);														//Genero el VAO
	glBindVertexArray(VAO);															//lo hago activo

	// GENERO EL VAO y le hago el bind para que este activo
	glGenBuffers(1, &VBO);															//Genero el indice del VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);												//Indico de que tipo es en este caso GL_ARRAY_BUFFER
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);		//Indico donde están los vertices en este caso se llama array verives
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);	// Indico el formato en que los vertices se encuentran el el array

	glEnableVertexAttribArray(0);													// Lo habilito


	glBindBuffer(GL_ARRAY_BUFFER, 0);												//  Desvinculo VBO
	glBindVertexArray(0);															// Desvinculo VAO
}

//Función con cosas que no hemos visto.
void openGlInit() {
	//Habilito aqui el depth en vez de arriba para los usuarios de linux y mac mejor arriba
	//Incializaciones varias
	glClearDepth(1.0f); //Valor z-buffer
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);  // valor limpieza buffer color
	glEnable(GL_DEPTH_TEST); // z-buffer
	glEnable(GL_CULL_FACE); //ocultacion caras back
	glCullFace(GL_BACK);
}
// Funcion main.

int main()
{
	//Genero ventana
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	   							 // Creo la ventana
							 // --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "DibujoEjes", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);


	//Leo el gald
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Fallo inicializacion Glad" << std::endl;
		return -1;
	}

	openGlInit();

	shaderProgram= setShaders("Ejes.vert", "Ejes.frag");					// leo los shaders
	medianteArray();														// Genero el VAO


	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);								// indico que quiero Dibujar lineas

	// Lazo principal
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// input
		// -----
		processInput(window);

		// render
		// ------
		//limpieza de los buffers
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);



		glUseProgram(shaderProgram);							 // leemos el programs shader
		glBindVertexArray(VAO);									//hacemos un bind al VAO que queremos emplear en el dibujo
		glDrawArrays(GL_LINES, 0, 6);							//Indicamos cuantos elementos del array tienen que ser dibujados
		//glDrawElements(GL_LINES, 6, GL_UNSIGNED_INT, 0);		// Utilianzo los indices para elVAO
		 glBindVertexArray(0);									// Ya no voy a usar ese VAO

		// limpio los buffers
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	//Como se acabo vacio los buffers
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	// Termino todo.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}



