/* Grua. Practica COGA 2025
 *
 * Autores:
 *      Hugo Coto Florez
 *      Guillermo Fernandez
 *
 * License: licenseless
 *
 * Mensaje del autor: El codigo hay que
 * disfrutarlo, no analizarlo.
 *
 * ---------------------------------*/

#include "include/glad/glad.h"
#include "include/glfw/include/GLFW/glfw3.h"
#include "include/glm/glm.hpp"
#include "include/glm/ext.hpp"

#include <cmath>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <vector>

#include "setShaders.h"

// im lazy
using namespace std;
using namespace glm;

/* Enable VSync. FPS limited to screen refresh rate
 * (0: disable, 1: enable, undef: default) */
#define VSYNC 0

// Return a RGB normalized coma separeted equivalent color
#define HexColor(hex_color)                     \
        ((hex_color & 0xFF0000) >> 16) / 256.0, \
        ((hex_color & 0xFF00) >> 8) / 256.0,    \
        ((hex_color & 0xFF)) / 256.0

#define BG_COLOR HexColor(0x87CEEB), 1.0

#define PI 3.1416f
#define PIMED 1.5708f

#define Point(a, b, c) a, b, c
#define Face4(a, b, c, d) a, b, c, a, c, d
#define vec_ptr(vec, i) ((vec).data() + (i))
#define SIZE(arr) (sizeof((arr)) / sizeof(*(arr)))

typedef struct Object {
        const char *name;
        GLuint vao;
        GLuint indexes_n;
        GLuint printable;
        int color;
        mat4 model; // relative model
        vector<Object *> attached;
        Object *parent;
} Object;

vector<Object> objects;

GLuint WIDTH = 640;
GLuint HEIGHT = 480;

unsigned int c_lock = 0;
GLuint shader_program;
float camera_pos_y = 24.0f;
float camera_pos_x = 48.0f;

/* As vel = vel_base / prev_fps,
 * if prev_fps is 0 in the first second
 * movement in infinity. Changing this variable
 * to max value (unsigned -1 )block movement until
 * prev_fps is calulated */
unsigned int prev_fps = -1;

enum {
        VIEW_3_PERSON,
        VIEW_1_PERSON,
        VIEW_NOFOLLOW,
} VIEW = VIEW_NOFOLLOW;

typedef enum {
        OBJ_GROUND = 0,
        OBJ_A,
        OBJ_BASE,
        OBJ_HEAD,
        OBJ_WHEEL_FL,
        OBJ_WHEEL_FR,
        OBJ_WHEEL_BL,
        OBJ_WHEEL_BR,
        OBJ_SPHERE_BASE,
        OBJ_SPHERE,
        OBJ_PALO,
} ObjectsId;

static inline vec3
get_model_position(mat4 model)
{
        return vec3(model[3]);
}

static inline mat3
get_model_rotation_matrix(mat4 model)
{
        return mat3(model);
}

static inline vec3
get_model_rotation(mat4 model)
{
        return eulerAngles(quat_cast(mat3(model)));
}

// Add a new objetc to global obj array
static void
new_object(const char *name, void(get_vao)(GLuint *, GLuint *),
           int color = 0x0, // black by default
           bool printable = 1) // printable by default
{
        GLuint vao, indexes_n;
        get_vao(&vao, &indexes_n);
        objects.push_back(
        (Object) {
        .name = name,
        .vao = vao,
        .indexes_n = indexes_n,
        .printable = printable,
        .color = color,
        .model = mat4(1.0f),
        .attached = vector<Object *>(),
        .parent = NULL,
        });
}

static inline void
show_help()
{
        printf("[W]: Moverse hacia delante\n");
        printf("[A]: Girar a la izquierda\n");
        printf("[S]: Moverse hacia atras\n");
        printf("[D]: Girar a la derecha\n");
        printf("[H]: Mover el cuerpo a la izquierda\n");
        printf("[J]: Mover la grua hacia abajo\n");
        printf("[K]: Mover la grua hacia arriba\n");
        printf("[L]: Mover el cuerpo a la derecha\n");
        printf("[C]: Cambiar camara\n");
        printf("[up]: Subir la camara\n");
        printf("[left]: Acercar la camara\n");
        printf("[down]: Bajar la camara\n");
        printf("[right]: Alejar la camara\n");
}

static void
vertex_to_vao(GLuint *VAO, size_t n, const float *vertex, size_t m, const GLuint *indexes,
              GLuint vertex_coords = 3, GLuint vertex_padding = 3)
{
        GLuint VBO, EBO;
        glGenVertexArrays(1, VAO);
        glBindVertexArray(*VAO);
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, n * sizeof(*vertex), vertex, GL_STATIC_DRAW);
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m * sizeof(*indexes), indexes, GL_STATIC_DRAW);
        glVertexAttribPointer(0, vertex_coords, GL_FLOAT, GL_FALSE, vertex_padding * sizeof(float), (void *) 0);

        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
}

void
generateSphere(float radius, unsigned int sectorCount, unsigned int stackCount,
               std::vector<float> &vertices, std::vector<unsigned int> &indices)
{
        vertices.clear();
        indices.clear();

        for (unsigned int i = 0; i <= stackCount; ++i) {
                float stackAngle = PI / 2 - i * (PI / stackCount);
                float xy = radius * cosf(stackAngle);
                float z = radius * sinf(stackAngle);

                for (unsigned int j = 0; j <= sectorCount; ++j) {
                        float sectorAngle = j * (2 * PI / sectorCount);
                        float x = xy * cosf(sectorAngle);
                        float y = xy * sinf(sectorAngle);

                        vertices.push_back(x);
                        vertices.push_back(y);
                        vertices.push_back(z);
                }
        }

        for (unsigned int i = 0; i < stackCount; ++i) {
                for (unsigned int j = 0; j < sectorCount; ++j) {
                        unsigned int first = i * (sectorCount + 1) + j;
                        unsigned int second = first + sectorCount + 1;

                        indices.push_back(first);
                        indices.push_back(second);
                        indices.push_back(first + 1);

                        indices.push_back(second);
                        indices.push_back(second + 1);
                        indices.push_back(first + 1);
                }
        }
}

void
sphere(GLuint *VAO, GLuint *indexes_n, float radius)
{
        std::vector<float> vertices;
        std::vector<unsigned int> indices;

        generateSphere(radius, 10, 10, vertices, indices);
        *indexes_n = indices.size();
        vertex_to_vao(VAO, vertices.size(), vertices.data(), indices.size(), indices.data());
}

void
square(GLuint *VAO, GLuint *indexes_n, float x)
{
        float vertices[] = {
                Point(-x / 2.0f, 0.0f, -x / 2.0f),
                Point(-x / 2.0f, 0.0f, x / 2.0f),
                Point(x / 2.0f, 0.0f, x / 2.0f),
                Point(x / 2.0f, 0.0f, -x / 2.0f),

                /* 3 ---- 2
                 * |      |
                 * |      |
                 * 0 ---- 1 */
        };

        unsigned int indices[] = {
                Face4(0, 1, 2, 3),
        };

        *indexes_n = SIZE(indices);
        vertex_to_vao(VAO, SIZE(vertices), vertices, SIZE(indices), indices);
}

/* Create a cube with a given (x, y, z) size */
static void
cube(GLuint *VAO, GLuint *indexes_n, float x, float y, float z)
{
        GLuint VBO, EBO;
        /*                                       | y
     0(-x,y,-z)-> /---------/|<- (x,y,-z)4       |
                 / |       / |                   |______ x
                /  |      /  |                  /
  1(-x,y,z)->  /_________/   |  <- (x,y,z)5    / z
 2(-x,-y,-z)-> |   /-----|---/ <- (x,-y,-z)6
               |  /      |  /
               | /       | /
  3(-x,-y,z)-> |/________|/ <- (x,-y,z)7


        */

        x /= 2;
        y /= 2;
        z /= 2;

        float vertices[] = {
                Point(-x, y, -z),
                Point(-x, y, z),
                Point(-x, -y, -z),
                Point(-x, -y, z),
                Point(x, y, -z),
                Point(x, y, z),
                Point(x, -y, -z),
                Point(x, -y, z),
        };

        GLuint indices[] = {
                Face4(0, 1, 5, 4), // top
                Face4(0, 2, 3, 1), // left
                Face4(6, 4, 5, 7), // right
                Face4(1, 3, 7, 5), // front
                Face4(0, 4, 6, 2), // back
                Face4(3, 2, 6, 7), // down
        };

        *indexes_n = SIZE(indices);
        vertex_to_vao(VAO, SIZE(vertices), vertices, SIZE(indices), indices);
}

static inline void
get_base_vao(GLuint *vao, GLuint *indexes_n)
{
        cube(vao, indexes_n, 7, 1.5, 3);
}

static inline void
get_head_vao(GLuint *vao, GLuint *indexes_n)
{
        cube(vao, indexes_n, 2, 3, 3.2);
}

static inline void
get_wheel_vao(GLuint *vao, GLuint *indexes_n)
{
        cube(vao, indexes_n, 1, 1, 0.5);
}

void
get_circle_base_vao(GLuint *vao, GLuint *indexes_n)
{
        cube(vao, indexes_n, 1.0, 1.0, 1.0);
}

void
get_circle_vao(GLuint *vao, GLuint *indexes_n)
{
        sphere(vao, indexes_n, 0.5);
}

void
get_palo_vao(GLuint *vao, GLuint *indexes_n)
{
        cube(vao, indexes_n, 0.5, 5.0, 0.5);
}

static inline void
get_ground_vao(GLuint *VAO, GLuint *indexes_n)
{
        square(VAO, indexes_n, 100);
}

static inline void
get_A_vao(GLuint *VAO, GLuint *indexes_n)
{
        cube(VAO, indexes_n, 1, 1, 1);
}

static void
process_input(GLFWwindow *window)
{
        float moveSpeed = 0.001f * 6000.0f / prev_fps;
        float cameraSpeed = 0.001f * 6000.0f / prev_fps;
        float rotateSpeed = 0.001f * 6000.0f / prev_fps;

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
                glfwSetWindowShouldClose(window, true);

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
                // move obj
                objects.at(OBJ_BASE).model = translate(objects.at(OBJ_BASE).model, vec3(moveSpeed, 0, 0));
                // rotate wheel
                objects.at(OBJ_WHEEL_FL).model = rotate(objects.at(OBJ_WHEEL_FL).model, moveSpeed, vec3(0, 0, -1));
                objects.at(OBJ_WHEEL_FR).model = rotate(objects.at(OBJ_WHEEL_FR).model, moveSpeed, vec3(0, 0, -1));
                objects.at(OBJ_WHEEL_BL).model = rotate(objects.at(OBJ_WHEEL_BL).model, moveSpeed, vec3(0, 0, -1));
                objects.at(OBJ_WHEEL_BR).model = rotate(objects.at(OBJ_WHEEL_BR).model, moveSpeed, vec3(0, 0, -1));
        }

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
                // move obj
                objects.at(OBJ_BASE).model = translate(objects.at(OBJ_BASE).model, vec3(-moveSpeed, 0, 0));
                // rotate wheel
                objects.at(OBJ_WHEEL_FL).model = rotate(objects.at(OBJ_WHEEL_FL).model, moveSpeed, vec3(0, 0, 1));
                objects.at(OBJ_WHEEL_FR).model = rotate(objects.at(OBJ_WHEEL_FR).model, moveSpeed, vec3(0, 0, 1));
                objects.at(OBJ_WHEEL_BL).model = rotate(objects.at(OBJ_WHEEL_BL).model, moveSpeed, vec3(0, 0, 1));
                objects.at(OBJ_WHEEL_BR).model = rotate(objects.at(OBJ_WHEEL_BR).model, moveSpeed, vec3(0, 0, 1));
        }

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
                objects.at(OBJ_BASE).model = rotate(objects.at(OBJ_BASE).model, rotateSpeed, vec3(0, 1, 0));
        }

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
                objects.at(OBJ_BASE).model = rotate(objects.at(OBJ_BASE).model, rotateSpeed, vec3(0, -1, 0));
        }

        if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
                if (get_model_rotation(objects.at(OBJ_SPHERE).model).z < PIMED)
                        objects.at(OBJ_SPHERE).model = rotate(objects.at(OBJ_SPHERE).model, rotateSpeed, vec3(0, 0, 1));
        }

        if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
                if (get_model_rotation(objects.at(OBJ_SPHERE).model).z > -PIMED)
                        objects.at(OBJ_SPHERE).model = rotate(objects.at(OBJ_SPHERE).model, rotateSpeed, vec3(0, 0, -1));
        }

        if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
                objects.at(OBJ_SPHERE_BASE).model = rotate(objects.at(OBJ_SPHERE_BASE).model, rotateSpeed, vec3(0, 1, 0));
        }

        if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
                objects.at(OBJ_SPHERE_BASE).model = rotate(objects.at(OBJ_SPHERE_BASE).model, rotateSpeed, vec3(0, -1, 0));
        }

        if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && !c_lock) {
                if (VIEW == VIEW_3_PERSON)
                        VIEW = VIEW_NOFOLLOW;
                else if (VIEW == VIEW_NOFOLLOW)
                        VIEW = VIEW_1_PERSON;
                else
                        VIEW = VIEW_3_PERSON;
                c_lock = 1;
        }

        if (glfwGetKey(window, GLFW_KEY_C) == GLFW_RELEASE && c_lock)
                c_lock = 0;

        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
                camera_pos_y += cameraSpeed;
        }

        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
                camera_pos_y -= cameraSpeed;
        }

        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
                camera_pos_x += cameraSpeed;
        }

        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
                camera_pos_x -= cameraSpeed;
        }

        if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS)
                for (struct Object &obj : objects)
                        obj.printable = 1;

#define __add_num(num)                                                                     \
        if (glfwGetKey(window, GLFW_KEY_0 + num) == GLFW_PRESS && objects.size() >= num) { \
                for (struct Object & obj : objects)                                        \
                        obj.printable = 0;                                                 \
                objects[num - 1].printable = 1;                                            \
        }

        __add_num(1);
        __add_num(2);
        __add_num(3);
        __add_num(4);
        __add_num(5);
        __add_num(6);
        __add_num(7);
        __add_num(8);
        __add_num(9);
}

static void
__framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
        WIDTH = width;
        HEIGHT = height;
        glViewport(0, 0, width, height);
}

static void
draw_object(Object object, mat4 model = mat4(1.0f))
{
        GLuint modelLoc = glGetUniformLocation(shader_program, "model");
        GLuint colorLoc = glGetUniformLocation(shader_program, "color");

        // get obj model
        model = model * object.model;

        // draw attached objects
        for (Object *attached_obj : object.attached)
                draw_object(*attached_obj, model);

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(model));
        glUniform3f(colorLoc, HexColor(object.color));

        if (object.printable == 0)
                return;

        glBindVertexArray(object.vao);
        glDrawElements(GL_TRIANGLES, object.indexes_n, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);
}

static void
attach_object(Object *parent, Object *child)
{
        if (child->parent)
                return;
        parent->attached.push_back(child);
        child->parent = parent;
}

static void
fps()
{
        static time_t last_time = 0;
        static unsigned int fps = 0;
        time_t time;

        time = glfwGetTime();
        ++fps;

        if (time - last_time >= 1) {
                last_time = time;
                printf("%u\n", fps);
                prev_fps = fps;
                fps = 0;
        }
}

/* Main loop. Executed until program is closed manually. */
int
mainloop(GLFWwindow *window)
{
        /* Execute until window is closed */
        while (!glfwWindowShouldClose(window)) {
                vec3 cameraEye;
                vec3 cameraPosition;
                vec3 cameraUp;
                mat4 view;
                mat4 projection = perspective(radians(45.0f), (float) WIDTH / (float) HEIGHT, 0.1f, 100.0f);

                fps();
                process_input(window);

                glClearColor(BG_COLOR);
                glClear(GL_COLOR_BUFFER_BIT);
                glClear(GL_DEPTH_BUFFER_BIT);

                mat4 model;
                vec3 dirf;
                switch (VIEW) {
                case VIEW_NOFOLLOW:
                        cameraEye = vec3(0, 0, 0);
                        cameraPosition = vec3(camera_pos_x, camera_pos_y, 0);
                        cameraUp = vec3(0.0f, 1.0f, 0.0f);
                        break;

                case VIEW_3_PERSON:
                        model = objects.at(OBJ_BASE).model;
                        dirf = normalize(vec3(model[0]));
                        cameraEye = get_model_position(model);
                        cameraPosition = cameraEye - dirf * camera_pos_x + vec3(0.0f, camera_pos_y, 0.0f);
                        cameraUp = vec3(model[1]);
                        break;

                case VIEW_1_PERSON:
                        model = objects.at(OBJ_BASE).model * (objects.at(OBJ_HEAD).model);
                        cameraPosition = get_model_position(model);
                        dirf = normalize(vec3(model[0]));
                        cameraEye = cameraPosition + dirf;
                        cameraUp = vec3(objects.at(OBJ_BASE).model[1]);
                        break;
                }

                view = lookAt(cameraPosition, cameraEye, cameraUp);

                glUseProgram(shader_program);

                GLuint viewLoc = glGetUniformLocation(shader_program, "view");
                GLuint projectionLoc = glGetUniformLocation(shader_program, "projection");
                glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(view));
                glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, value_ptr(projection));

                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                draw_object(objects.at(OBJ_GROUND));
                draw_object(objects.at(OBJ_A));
                draw_object(objects.at(OBJ_BASE));
                /* Every object attached to OBJ_BASE is automatically drawn */

                glfwSwapBuffers(window);
                glfwPollEvents();
        }

        for (struct Object &obj : objects) {
                glDeleteVertexArrays(1, &obj.vao);
        }

        return 0;
}

void
init_objects()
{
        new_object("ground", get_ground_vao, 0xbbbbbb);
        new_object("A", get_A_vao, 0xFFFFFF);
        new_object("base", get_base_vao, 0x5d6652);
        new_object("head", get_head_vao, 0xbeaf61);
        new_object("wheel_fl", get_wheel_vao, 0x444444);
        new_object("wheel_fr", get_wheel_vao, 0x444444);
        new_object("wheel_bl", get_wheel_vao, 0x444444);
        new_object("wheel_br", get_wheel_vao, 0x444444);
        new_object("circle_base", get_circle_base_vao, 0xbeaf61);
        new_object("circle", get_circle_vao, 0x8695b3);
        new_object("palo", get_palo_vao, 0xd9deb4);

        // A
        objects.at(OBJ_A).model = translate(objects.at(OBJ_A).model, vec3(6.0, 3, 0.0));

        // Base
        objects.at(OBJ_BASE).model = rotate(objects.at(OBJ_BASE).model, PIMED, vec3(0.0, 1.0, 0.0));
        objects.at(OBJ_BASE).model = translate(objects.at(OBJ_BASE).model, vec3(0.0, 1.39, 0.0));

        // Head
        attach_object(vec_ptr(objects, OBJ_BASE), vec_ptr(objects, OBJ_HEAD));
        objects.at(OBJ_HEAD).model = translate(objects.at(OBJ_HEAD).model, vec3(2.75, 2.0, 0.0));

        // wheel_fl
        attach_object(vec_ptr(objects, OBJ_BASE), vec_ptr(objects, OBJ_WHEEL_FL));
        objects.at(OBJ_WHEEL_FL).model = translate(objects.at(OBJ_WHEEL_FL).model, vec3(2.5, -0.75, -1.5));

        // wheel_fr
        attach_object(vec_ptr(objects, OBJ_BASE), vec_ptr(objects, OBJ_WHEEL_FR));
        objects.at(OBJ_WHEEL_FR).model = translate(objects.at(OBJ_WHEEL_FR).model, vec3(2.5, -0.75, +1.5));

        // wheel_bl
        attach_object(vec_ptr(objects, OBJ_BASE), vec_ptr(objects, OBJ_WHEEL_BL));
        objects.at(OBJ_WHEEL_BL).model = translate(objects.at(OBJ_WHEEL_BL).model, vec3(-2.5, -0.75, -1.5));

        // wheel_br
        attach_object(vec_ptr(objects, OBJ_BASE), vec_ptr(objects, OBJ_WHEEL_BR));
        objects.at(OBJ_WHEEL_BR).model = translate(objects.at(OBJ_WHEEL_BR).model, vec3(-2.5, -0.75, +1.5));

        // sphere base
        attach_object(vec_ptr(objects, OBJ_BASE), vec_ptr(objects, OBJ_SPHERE_BASE));
        objects.at(OBJ_SPHERE_BASE).model = translate(objects.at(OBJ_SPHERE_BASE).model, vec3(-1.0, 0.75, 0.0));

        // sphere
        attach_object(vec_ptr(objects, OBJ_SPHERE_BASE), vec_ptr(objects, OBJ_SPHERE));
        objects.at(OBJ_SPHERE).model = translate(objects.at(OBJ_SPHERE).model, vec3(0.0, 0.5, 0.0));

        // palo
        attach_object(vec_ptr(objects, OBJ_SPHERE), vec_ptr(objects, OBJ_PALO));
        objects.at(OBJ_PALO).model = translate(objects.at(OBJ_PALO).model, vec3(0.0, 2.75, 0.0));
}


/* Entry point: Initialize stuff and then enter mainloop. */
int
main(int argc, char **argv)
{
        if (!glfwInit()) {
                fprintf(stderr, "Can not init glfw\n");
                exit(1);
        }
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        // GLFWmonitor *monitor = glfwGetPrimaryMonitor(); // fullscreen
        GLFWmonitor *monitor = NULL; // floating (or not)
        GLFWwindow *share = NULL;
        GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Titulo", monitor, share);

        if (window == NULL) {
                perror("glfwCreateWindow");
                glfwTerminate(); // terminate initialized glfw
                return 1;
        }

        glfwMakeContextCurrent(window);
        glfwSetFramebufferSizeCallback(window, __framebuffer_size_callback);

#if defined(VSYNC)
        glfwSwapInterval(VSYNC);
#endif

        if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
                fprintf(stderr, "gladLoadGLLoader failed");
                glfwTerminate();
                return 1;
        }

        glClearDepth(1.0f);
        glClearColor(BG_COLOR);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        shader_program = setShaders("vertex_shader.glsl", "fragment_shader.glsl");

        show_help();

        init_objects();

        mainloop(window);
        glfwTerminate();

        return 0;
}
