#include <stdio.h>
#include <stdlib.h>
#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glad/glad.h"
#include "linmath.h"

void error_callback(int error, const char * description) {
  fprintf(stderr, "Error: %s\n", description);
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void input(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

void render(GLFWwindow * window) {
    glDrawArrays(GL_LINES, 0, 2);

}

GLuint generateShader() {
    const char *vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec2 aPos;\n"
        "uniform mat4 MVP;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = MVP * vec4(aPos.x, aPos.y, 0.0, 1.0);\n"
        "}\0";
    const char *fragmentShaderSource = "#version 330 core\n"
        "out vec4 FragColour;\n"
        "uniform vec3 colour;\n"
        "void main()\n"
        "{\n"
        "   FragColour = vec4(colour, 1.0f);\n"
        "}\n\0";

    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    int success;
    char info_log[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, info_log);
        fprintf(stderr, "[ERROR] SHADER COMPILATION FAILED\n%s", info_log);
    }

    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, info_log);
        fprintf(stderr, "[ERROR] SHADER COMPILATION FAILED\n%s", info_log);
    }

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
      glGetProgramInfoLog(shaderProgram, 512, NULL, info_log);
      fprintf(stderr, "[ERROR] LINKING SHADER PROGRAM FAILED\n%s", info_log);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return shaderProgram;
}

typedef struct {
    unsigned int x;
    unsigned int y;
    unsigned int VAO;
    unsigned int VBO;
} hcGrid;

void hcGridInit(hcGrid* grid, unsigned int x, unsigned int y) {
    grid->x = x; grid->y = y;
    float* data = malloc(4*(x+y)*sizeof(float));

    // float data[4*(x+y)];
    {
        float xRight = (float)x/2.0f;
        float yInitial = -((float)y-1)/2.0f;
        float yTop = (float)y/2.0f; 
        float xInitial = -((float)x-1)/2.0f;
        for (int i=0; i<x; i++) {
            data[i*4] = -xRight;
            data[(i*4)+1] = yInitial+i; 
            data[(i*4)+2] = xRight;
            data[(i*4)+3] = yInitial+i; 
        }    
        for (int j=0; j<y; j++) {
            data[x*4+j*4] = xInitial+j;
            data[x*4+j*4+1] = -yTop; 
            data[x*4+j*4+2] = xInitial+j;
            data[x*4+j*4+3] = yTop; 
        }    

    }

    glGenVertexArrays(1, &grid->VAO);
    glBindVertexArray(grid->VAO);

    glGenBuffers(1, &grid->VBO);
    glBindBuffer(GL_ARRAY_BUFFER, grid->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*4*(x+y), data, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(GL_NONE);
    free(data);
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

    gladLoadGL();
    glViewport(0, 0, 900, 900);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    hcGrid grid;
    unsigned int k = 100;
    hcGridInit(&grid, k, k);

    GLuint shaderProgram = generateShader();
    glUseProgram(shaderProgram);
    int mvpLoc = glGetUniformLocation(shaderProgram, "MVP");

    while(!glfwWindowShouldClose(window)) {
        // main loop
        input(window);

        glClearColor(0.2f, 0.3f, 0.6f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(grid.VAO);

        mat4x4 m;
        mat4x4_identity(m);
        mat4x4_scale_aniso(m, m, 2.0f/k, 2.0f/k, 2.0f/k);
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, (const GLfloat *)m);

        glDrawArrays(GL_LINES, 0, 2*(grid.x+grid.y));

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    // cleanup
    glDeleteVertexArrays(1, &grid.VAO);
    glDeleteBuffers(1, &grid.VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}
