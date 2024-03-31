#include <stdio.h>
#include <stdlib.h>
#include "grid.h"
#include "settings.h"

GLuint generateShader() {
    unsigned int r;
    char * vertexShaderSource = loadFile(settings.vertex_shader_path, &r);
    char * fragmentShaderSource = loadFile(settings.fragment_shader_path, &r);

    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, (const char *const *)&vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    int success;
    char info_log[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, info_log);
        fprintf(stderr, "[ERROR] SHADER COMPILATION FAILED\n%s", info_log);
    }

    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, (const char * const *)&fragmentShaderSource, NULL);
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


    free(vertexShaderSource);
    free(fragmentShaderSource);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return shaderProgram;
}


void hcGridInit(hcGrid* grid) {

    grid->shaderProgram = generateShader();
    glUseProgram(grid->shaderProgram);
    grid->mvpLoc = glGetUniformLocation(grid->shaderProgram, "MVP");
    grid->colourLoc = glGetUniformLocation(grid->shaderProgram, "colour");

    grid->x = settings.grid_x; grid->y = settings.grid_y;
    unsigned int x = grid->x; unsigned int y = grid->y;
    float* data = malloc(4*(x+y)*sizeof(float));

    // float data[4*(x+y)];
    {
        float xRight = (float)y/2.0f;
        float yInitial = -((float)x-1)/2.0f;
        float yTop = (float)x/2.0f; 
        float xInitial = -((float)y-1)/2.0f;
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

void hcGridDraw(hcGrid* grid) {
    mat4x4 m;
    mat4x4_identity(m);
    mat4x4_scale_aniso(m, m, settings.scale/grid->x, settings.scale/grid->x, settings.scale/grid->x);

    // opengl
    glUseProgram(grid->shaderProgram);
    glBindVertexArray(grid->VAO);

    glUniformMatrix4fv(grid->mvpLoc, 1, GL_FALSE, (const GLfloat *)m);
    glUniform3f(grid->colourLoc, (sin(glfwGetTime())+1.0f)/2.0f, 1.0f, 1.0f);

    glDrawArrays(GL_LINES, 0, 2*(grid->x+grid->y));

    glUseProgram(GL_NONE);
    glBindVertexArray(GL_NONE);
}

void hcGridDelete(hcGrid* grid) {
    glDeleteVertexArrays(1, &grid->VAO);
    glDeleteBuffers(1, &grid->VBO);
    glDeleteProgram(grid->shaderProgram);
}
