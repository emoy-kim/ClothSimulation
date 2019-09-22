#pragma once

#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <gl/FreeImage.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#pragma comment(lib,"glfw3dll.lib")
#pragma comment(lib,"FreeImage.lib")

#ifdef _DEBUG
#pragma comment(lib,"glew32d.lib")
#else
#pragma comment(lib,"glew32.lib")

#endif