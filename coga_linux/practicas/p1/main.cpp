#include <glad/glad.h> // have to be included before glfw3.h

#include "setShalders.h"
#include <GL/gl.h> // also included in glad
#include <GLFW/glfw3.h> // also included in glad
#include <assert.h>
#include <stdio.h> // fuck u iostream

#define WIDTH 600
#define HEIGHT 480

/* Global to made easy share it among funtions */
GLuint shader_program;
unsigned int VAO, VBO, EBO;

int mainloop(GLFWwindow *window);

/* This funtion is going to be executed in main loop
 * to catch input and do whatever is needed */
void
__process_input(GLFWwindow *window)
{
        /* TODO: I think this shit do not work in linux. Idk
         * why GLFW_KEY_ESCAPE is 256 as this is del not esc */
        // check if escape is pressed
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
                glfwSetWindowShouldClose(window, true);
}

/* This function is called on window resize */
void
__framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
        // make sure the viewport matches the new window dimensions; note that width and
        // height will be significantly larger than specified on retina displays.
        glViewport(0, 0, width, height);
}

void
__using_vertex_color_array()
{
        unsigned int VBO, EBO; /* Declare buffer objects for vertices (VBO) and indices (EBO) */

        // Define vertex data with positions and colors
        float vertices[] = {
                // First triangle (0, 1, 2)
                -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // Bottom left (red)
                0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // Bottom right (green)
                0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, // Top right (blue)

                // Second triangle (0, 2, 3)
                -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // Bottom left (red)
                0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, // Top right (blue)
                -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f // Top left (yellow)
        };
        /* Generate a Vertex Array Object (VAO) to store
         * vertex attribute configuration */
        glGenVertexArrays(1, &VAO);

        /* Generate a Vertex Buffer Object (VBO) to store
         * vertex data */
        glGenBuffers(1, &VBO);

        /* Bind the VAO (this stores attribute
         * configurations) */
        glBindVertexArray(VAO);

        /* Bind the VBO and upload vertex data to GPU memory */
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        /* Define the vertex position attribute
         * (location = 0 in the shader) */
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) 0);
        /* Enable the attribute */
        glEnableVertexAttribArray(0);

        /* Define the vertex color attribute
         * (location = 1 in the shader) */
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                              (void *) (3 * sizeof(float)));
        /* Enable the attribute */
        glEnableVertexAttribArray(1);

        /* Unbind the VBO
         * (optional, since it's stored in VAO) */
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        /* Unbind the VAO
         * (not strictly necessary, but good practice) */
        glBindVertexArray(0);

        /* Delete the VBO after usage
         * (data remains stored in GPU memory due to VAO) */
        glDeleteBuffers(1, &VBO);
}

void
__using_vertex_array()
{
        /*
         * Defines the vertex data for rendering.
         * This array stores the positions of the vertices in 3D space.
         * Each group of three floats represents a vertex (x, y, z).
         */
        // float vertices[] = {
        //         0.0f, 0.0f, -0.5f, // First vertex (x, y, z)
        //         0.5f, 0.0f, -0.5f, // Second vertex
        //         0.0f, 0.5f, -0.5f, // Third vertex
        //         0.0f, 0.0f, 0.5f // Fourth vertex (if needed for another shape)
        // };

        /* Cuadrado sin usar EBO (con repeticion de vertices)
         * Se necesita definir dos triangulos. */
        float vertices[] = {
                -0.5f, -0.5f, 0.0f, // Vértice 0 - Inferior izquierdo
                0.5f,  -0.5f, 0.0f, // Vértice 1 - Inferior derecho
                0.5f,  0.5f,  0.0f, // Vértice 2 - Superior derecho

                -0.5f, -0.5f, 0.0f, // Vértice 3 (Repetido) - Inferior izquierdo
                0.5f,  0.5f,  0.0f, // Vértice 4 (Repetido) - Superior derecho
                -0.5f, 0.5f,  0.0f // Vértice 5 - Superior izquierdo
        };

        /* Generates a Vertex Array Object (VAO) and binds it.
         * - VAO stores vertex attribute configurations and
         *   buffer bindings. It must be bound before
         *   configuring any vertex attributes. */
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        /* Generates a Vertex Buffer Object (VBO) and binds it.
         * - VBO stores the vertex data in GPU memory.
         * - `GL_ARRAY_BUFFER`: Specifies that this buffer
         *   holds vertex attributes. */
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        /* Allocates and copies vertex data into the VBO.
         * - `sizeof(vertices)`: Size of the data in bytes.
         * - `vertices`: Pointer to the vertex data.
         * - `GL_STATIC_DRAW`: Data is set once and used
         *   many times (optimized for performance). */
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        /*
         * Defines how OpenGL should interpret the vertex
         * data.
         * - `0`: Attribute location in the shader
         *   (layout location 0).
         * - `3`: Number of components per vertex (x, y, z).
         * - `GL_FLOAT`: Data type of each component.
         * - `GL_FALSE`: No normalization.
         * - `3 * sizeof(float)`: Stride (distance between
         *   consecutive vertices).
         * - `(void *)0`: Offset in the buffer
         *   (starts at the beginning).
         */
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);

        /* Enables the vertex attribute at location 0.
         * This allows OpenGL to use the vertex data
         * when rendering. */
        glEnableVertexAttribArray(0);

        /* Unbinds the VBO to prevent accidental
         * modification. */
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        /* Unbinds the VAO to avoid modifying it
         * unintentionally. This is good practice when
         * working with multiple VAOs. */
        glBindVertexArray(0);
}

void
__using_vertex_indexes()
{
        float vertices[] = {
                -0.5f, -0.5f, 0.0f, // Vértice 0 - Inferior izquierdo
                0.5f,  -0.5f, 0.0f, // Vértice 1 - Inferior derecho
                0.5f,  0.5f,  0.0f, // Vértice 2 - Superior derecho
                -0.5f, 0.5f,  0.0f // Vértice 5 - Superior izquierdo
        };
        unsigned int indices[] = {
                0, 1, 2, // triangulo 1
                2, 3, 0 // triangulo 2
        };

        /* ----[ VAO ]---- */
        /* Generates a Vertex Array Object (VAO) and binds it.
         * - VAO stores vertex attribute configurations and
         *   buffer bindings. It must be bound before
         *   configuring any vertex attributes. */
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        /* ----[ VBO ]---- */
        /* Generates a Vertex Buffer Object (VBO) and binds it.
         * - VBO stores the vertex data in GPU memory.
         * - `GL_ARRAY_BUFFER`: Specifies that this buffer
         *   holds vertex attributes. */
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        /* Allocates and copies vertex data into the VBO.
         * - `sizeof(vertices)`: Size of the data in bytes.
         * - `vertices`: Pointer to the vertex data.
         * - `GL_STATIC_DRAW`: Data is set once and used
         *   many times (optimized for performance). */
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        /* ----[ EBO ]---- */
        /* Generates an Element Buffer Object (EBO) and binds it.
         * - EBO stores indices that define the order of vertices.
         * - `GL_ELEMENT_ARRAY_BUFFER`: Specifies that this buffer
         *   holds indices for indexed drawing. */
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

        /* Allocates and copies index data into the EBO.
         * - `sizeof(indices)`: Size of the index data in bytes.
         * - `indices`: Pointer to the index data.
         * - `GL_STATIC_DRAW`: Data is set once and used
         *   many times (optimized for performance). */
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        /* ----[ ~~~ ]---- */

        /*
         * Defines how OpenGL should interpret the vertex
         * data.
         * - `0`: Attribute location in the shader
         *   (layout location 0).
         * - `3`: Number of components per vertex (x, y, z).
         * - `GL_FLOAT`: Data type of each component.
         * - `GL_FALSE`: No normalization.
         * - `3 * sizeof(float)`: Stride (distance between
         *   consecutive vertices).
         * - `(void *)0`: Offset in the buffer
         *   (starts at the beginning).
         */
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);

        /* Enables the vertex attribute at location 0.
         * This allows OpenGL to use the vertex data
         * when rendering. */
        glEnableVertexAttribArray(0);

        /* Unbinds the VBO to prevent accidental
         * modification. Not neccesary */
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        /* Unbinds the VAO to avoid modifying it
         * unintentionally. This is good practice when
         * working with multiple VAOs. */
        glBindVertexArray(0);
}

void
__using_vertex_color_indexes()
{
        float vertices[] = {
                -0.5f, -0.5f, 0.0f, // Vértice 0 - Inferior izquierdo
                1.0f,  1.0f,  1.0f, // color
                0.5f,  -0.5f, 0.0f, // Vértice 1 - Inferior derecho
                1.0f,  0.0f,  0.0f, // color
                0.5f,  0.5f,  0.0f, // Vértice 2 - Superior derecho
                0.0f,  1.0f,  0.0f, // color
                -0.5f, 0.5f,  0.0f, // Vértice 3 - Superior izquierdo
                0.0f,  0.0f,  1.0f // color
        };
        unsigned int indices[] = {
                0, 1, 2, // triangulo 1
                2, 3, 0 // triangulo 2
        };

        /* ----[ VAO ]---- */
        /* Generates a Vertex Array Object (VAO) and binds it.
         * - VAO stores vertex attribute configurations and
         *   buffer bindings. It must be bound before
         *   configuring any vertex attributes. */
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        /* ----[ VBO ]---- */
        /* Generates a Vertex Buffer Object (VBO) and binds it.
         * - VBO stores the vertex data in GPU memory.
         * - `GL_ARRAY_BUFFER`: Specifies that this buffer
         *   holds vertex attributes. */
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        /* Allocates and copies vertex data into the VBO.
         * - `sizeof(vertices)`: Size of the data in bytes.
         * - `vertices`: Pointer to the vertex data.
         * - `GL_STATIC_DRAW`: Data is set once and used
         *   many times (optimized for performance). */
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        /* ----[ EBO ]---- */
        /* Generates an Element Buffer Object (EBO) and binds it.
         * - EBO stores indices that define the order of vertices.
         * - `GL_ELEMENT_ARRAY_BUFFER`: Specifies that this buffer
         *   holds indices for indexed drawing. */
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

        /* Allocates and copies index data into the EBO.
         * - `sizeof(indices)`: Size of the index data in bytes.
         * - `indices`: Pointer to the index data.
         * - `GL_STATIC_DRAW`: Data is set once and used
         *   many times (optimized for performance). */
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        /* ----[ ~~~ ]---- */

        /*
         * Defines how OpenGL should interpret the vertex
         * data.
         * - `0`: Attribute location in the shader
         *   (layout location 0).
         * - `3`: Number of components per vertex (x, y, z).
         * - `GL_FLOAT`: Data type of each component.
         * - `GL_FALSE`: No normalization.
         * - `3 * sizeof(float)`: Stride (distance between
         *   consecutive vertices).
         * - `(void *)0`: Offset in the buffer
         *   (starts at the beginning).
         */
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) 0);

        /* Enables the vertex attribute at location 0.
         * This allows OpenGL to use the vertex data
         * when rendering. */
        glEnableVertexAttribArray(0);

        /* Enable colors at offset 3*sizeof(float) */
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                              (void *) (3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        /* Unbinds the VBO to prevent accidental
         * modification. Not neccesary */
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        /* Unbinds the VAO to avoid modifying it
         * unintentionally. This is good practice when
         * working with multiple VAOs. */
        glBindVertexArray(0);
}

/* Entry point: Initialize stuff and then enter mainloop. */
int
main()
{
        /* ---- Init GLFW ---- */
        glfwInit();
        // just info that I dont know if it is needed
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        // TODO: Creo que me descarque el otro
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        /* ---- Create the main window ---- */
        // Monitor = NULL
        // Share = NULL
        GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Titulo", NULL, NULL);

        if (window == NULL)
        {
                perror("glfwCreateWindow");
                glfwTerminate(); // terminate initialized glfw
                return 1;
        }

        /* @brief Makes the context of the specified window
         * current for the calling thread.
         * This function makes the OpenGL or OpenGL ES
         * context of the specified window current on the
         * calling thread. It can also detach the current
         * context from the calling thread without making a
         * new one current by passing in `NULL`. */
        glfwMakeContextCurrent(window);

        /* Call __framebuffer_size_callback() on window resize */
        glfwSetFramebufferSizeCallback(window, __framebuffer_size_callback);

        /* You can use prints, it is going to be printed
         * to the terminal not to the window. It is needed to
         * flush prints, by fflush or '\n' */
        printf("Some printf\n");

        /* Load the GLAD. IDK what is this */
        if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
        {
                perror("gladLoadGLLoader");
                /* In main1_33a.cpp it just return but I think
                 * it is needed to call glfwTerminate(). */
                return 1;
        }

        // z-buffer value
        glClearDepth(1.0f);
        // clear buffer color
        glClearColor(.0f, .0f, .0f, 1.0f);
        // enable z-buffer
        glEnable(GL_DEPTH_TEST);
        // hide back faces
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        /* Program shader */
        shader_program =
        setShaders("vertex_shader.glsl", "fragment_shader.glsl");


        /* Sets the polygon rasterization mode for rendering.
         * This determines how OpenGL draws polygons.
         *
         * Options for `face`:
         * - `GL_FRONT`: Applies the mode to front-facing polygons only.
         * - `GL_BACK`: Applies the mode to back-facing polygons only.
         * - `GL_FRONT_AND_BACK`: Applies the mode to both front and back faces.
         *
         * Options for `mode`:
         * - `GL_POINT`: Renders only the vertices as points.
         * - `GL_LINE`: Renders only the edges as wireframes.
         * - `GL_FILL`: Renders solid, filled polygons (default).  */
        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


        // __using_vertex_array();
        // __using_vertex_color_array();
        //__using_vertex_indexes();
        __using_vertex_color_indexes();


        return mainloop(window);
}

/* Main loop. Executed until program is closed manually. */
int
mainloop(GLFWwindow *window)
{
        /* Execute until window is closed */
        while (!glfwWindowShouldClose(window))
        {
                // Call our process input function
                __process_input(window);

                // Clear buffers
                glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // BG
                glClear(GL_COLOR_BUFFER_BIT);
                glClear(GL_DEPTH_BUFFER_BIT);

                // Use shader program
                glUseProgram(shader_program);

                /* Binds the specified Vertex Array Object
                 * (VAO).
                 * This ensures that subsequent vertex
                 * operations use the correct VAO
                 * configuration.
                 * Without this, OpenGL wouldn't know which
                 * vertex data to use. */
                glBindVertexArray(VAO);

                /* Renders primitives (lines, triangles, etc.)
                 * using vertex data in order.
                 * - `GL_LINES`: Draws lines, each pair of
                 *   vertices forms a line.
                 * - `0`: Starts from the first vertex in
                 *   the buffer.
                 * - `6`: Number of vertices to process
                 *   (draws 3 lines).
                 * Use this when you don't need indexed
                 * drawing. */
                // glDrawArrays(GL_LINES, 0, 6);
                // glDrawArrays(GL_TRIANGLES, 0, 6); // for square

                /* Renders primitives using indexed drawing
                 * with an Element Buffer Object (EBO).
                 * - `GL_LINES`: Draws lines using the
                 *   specified indices.
                 * - `6`: Number of indices to read from the
                 *   EBO.
                 * - `GL_UNSIGNED_INT`: Type of the indices
                 *   in the EBO.
                 * - `0`: Start at the beginning of the EBO.
                 * Use this when vertices are reused to
                 * optimize memory usage. */
                // glDrawElements(GL_LINES, 6, GL_UNSIGNED_INT, 0);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

                /* Unbinds the currently active VAO.
                 * This prevents unintended modifications to
                 * the VAO.
                 * It's a good practice when working with
                 * multiple VAOs. */
                glBindVertexArray(0);

                /* @brief Swaps the front and back buffers of
                 * the specified window.
                 * This function swaps the front and back
                 * buffers of the specified window when
                 * rendering with OpenGL or OpenGL ES.
                 * If the swap interval is greater than zero,
                 * the GPU driver waits the specified number of
                 * screen updates before swapping the buffers. */
                /* ~~ load new frame */
                glfwSwapBuffers(window);

                /* @brief Processes all pending events.
                 * This function processes only those events
                 * that are already in the event queue and
                 * then returns immediately. Processing
                 * events will cause the window and input
                 * callbacks associated with those events
                 * to be called. */
                /* It does not block */
                glfwPollEvents();
        }

        /* Deletes the specified Vertex Array Object (VAO).
         * This frees the GPU memory associated with the VAO.
         * It's important to call this when the VAO is no
         * longer needed to avoid memory leaks.
         */
        glDeleteVertexArrays(1, &VAO);

        /* Deletes the specified Vertex Buffer Object (VBO).
         * This releases the allocated GPU memory for vertex
         * data. Deleting unused buffers
         * helps optimize resource usage.
         */
        glDeleteBuffers(1, &VBO);

        glfwTerminate();
        return 0;
}
