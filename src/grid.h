#ifndef GRID_H
#define GRID_H

#include "linmath.h"
#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glad/glad.h"
#include "settings.h"

typedef struct {
    unsigned int x;
    unsigned int y;
    unsigned int VAO;
    unsigned int VBO;
    GLuint shaderProgram;
    unsigned int mvpLoc;
    unsigned int colourLoc;
    mat4x4 mv;
} hcGrid;

void hcGridInit(hcGrid* grid);
void hcGridDraw(hcGrid* grid);
void hcGridDelete(hcGrid* grid);

#endif
