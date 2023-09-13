#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <soil2/SOIL2.h>

// Main configs
// GLOBAL GAME WINDOW SIZE
#define SIZE_Init 640
// Usefull values for processing
extern int Scale;
extern float Dimention;
extern float XZero;
extern float YZero;
//

// Enable
extern void OpenGLEnable(GLenum ToEnable);
//Disable
extern void OpenGLDisable(GLenum ToDisable);

//PolygonMode
extern void OpenGLPolygonMode(GLenum Face, GLenum Mode);

//Blending
extern void OpenGLBlend(GLenum SFactor, GLenum DFactor);

//GLRendering
extern void OpenGLRendering(GLenum Cull, GLenum Front);