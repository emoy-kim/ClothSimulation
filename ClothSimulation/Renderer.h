/*
 * Author: Emoy Kim
 * E-mail: emoy.kim_AT_gmail.com
 * 
 * This code is a free software; it can be freely used, changed and redistributed.
 * If you use any version of the code, please reference the code.
 * 
 */

#pragma once

#include "_Common.h"

#include "Camera.h"
#include "Object.h"

class RendererGL
{
   static RendererGL* Renderer;
   GLFWwindow* Window;

   vec2 ClickedPoint;
   uint ClothTargetIndex;
   ivec2 ClothPointNumSize;
   ivec2 ClothGridSize;
   vec3 SpherePosition;
   float SphereRadius;
   mat4 ClothWorldMatrix;
   mat4 SphereWorldMatrix;

   shared_ptr<CameraGL> MainCamera;
   shared_ptr<ShaderGL> ObjectShader;
   
   shared_ptr<LightGL> Lights;
   shared_ptr<ObjectGL> ClothObject, SphereObject;

   void registerCallbacks() const;
   void initialize();
   void printOpenGLInformation() const;

   void cleanup(GLFWwindow* window);
   void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods);
   void cursor(GLFWwindow* window, double xpos, double ypos);
   void mouse(GLFWwindow* window, int button, int action, int mods);
   void mousewheel(GLFWwindow* window, double xoffset, double yoffset);
   void reshape(GLFWwindow* window, int width, int height);
   static void cleanupWrapper(GLFWwindow* window);
   static void keyboardWrapper(GLFWwindow* window, int key, int scancode, int action, int mods);
   static void cursorWrapper(GLFWwindow* window, double xpos, double ypos);
   static void mouseWrapper(GLFWwindow* window, int button, int action, int mods);
   static void mousewheelWrapper(GLFWwindow* window, double xoffset, double yoffset);
   static void reshapeWrapper(GLFWwindow* window, int width, int height);

   void setLights();
   void setClothObject() const;
   void setSphereObject() const;
   void setClothPhysicsVariables() const;

   void applyForces();
   void drawClothObject(ShaderGL* shader, CameraGL* camera);
   void drawSphereObject(ShaderGL* shader, CameraGL* camera) const;
   void render();


public:
   RendererGL(const RendererGL&) = delete;
   RendererGL(const RendererGL&&) = delete;
   RendererGL& operator=(const RendererGL&) = delete;
   RendererGL& operator=(const RendererGL&&) = delete;


   RendererGL();
   ~RendererGL();

   void play();
};