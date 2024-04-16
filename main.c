#include <stdio.h>
#include <stdlib.h>
#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glad/glad.h"
#define EXTERN
#include "grid.h"

hcGrid grid;

void error_callback(int error, const char * description) {
  fprintf(stderr, "Error: %s\n", description);
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_R && action == GLFW_PRESS) {
        printf("%s", "reloading settings\n");
        loadSettings();
        hcGridDelete(&grid);
        hcGridInit(&grid);
    }
}

void input(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

int main(int argv, char ** args) {

    if(!glfwInit()) {
        exit(EXIT_FAILURE);
    }

    glfwSetErrorCallback(error_callback);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(900, 900, "hashi", NULL, NULL);
    if (window == NULL) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwMakeContextCurrent(window);

    loadSettings();

	gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    glViewport(0, 0, 900, 900);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);

    hcGridInit(&grid);

    while(!glfwWindowShouldClose(window)) {
        // main loop
        input(window);

        glClearColor(0.2f, 0.3f, 0.6f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        hcGridDraw(&grid);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    // cleanup
    hcGridDelete(&grid);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}
