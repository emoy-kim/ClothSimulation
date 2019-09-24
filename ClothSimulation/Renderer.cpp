#include "Renderer.h"

RendererGL* RendererGL::Renderer = nullptr;
RendererGL::RendererGL() :
   Window( nullptr ), ClickedPoint( -1, -1 ), ClothTargetIndex( 0 ), ClothPointNumSize( 100, 100 ),
   ClothGridSize( 150, 150 ), SpherePosition( 75.0f, 0.0f, 70.0f ), SphereRadius( 20.0f ),
   ClothWorldMatrix( translate( mat4(1.0f), vec3(0.0f, 100.0f, 0.0f) ) ),
   SphereWorldMatrix( translate( mat4(1.0f), vec3(75.0f, 0.0f, 70.0f) ) )
{
   Renderer = this;
   MainCamera = make_shared<CameraGL>();
   ObjectShader = make_shared<ShaderGL>();
   Lights = make_shared<LightGL>();
   ClothObject = make_shared<ObjectGL>();
   SphereObject = make_shared<ObjectGL>();
   SuzanneObject = make_shared<ObjectGL>();

   initialize();
   printOpenGLInformation();
}

RendererGL::~RendererGL()
{
   glfwTerminate();
}

void RendererGL::printOpenGLInformation() const
{
   cout << "****************************************************************" << endl;
   cout << " - GLFW version supported: " << glfwGetVersionString() << endl;
   cout << " - GLEW version supported: " << glewGetString( GLEW_VERSION ) << endl;
   cout << " - OpenGL renderer: " << glGetString( GL_RENDERER ) << endl;
   cout << " - OpenGL version supported: " << glGetString( GL_VERSION ) << endl;
   cout << " - OpenGL shader version supported: " << glGetString( GL_SHADING_LANGUAGE_VERSION ) << endl  ;
   cout << "****************************************************************" << endl << endl;
}

void RendererGL::initialize()
{
   if (!glfwInit()) {
      cout << "Cannot Initialize OpenGL..." << endl;
      return;
   }
   glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
   glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 5 );
   glfwWindowHint( GLFW_DOUBLEBUFFER, GLFW_TRUE );
   glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

   const int width = 1920;
   const int height = 1080;
   Window = glfwCreateWindow( width, height, "Main Camera", nullptr, nullptr );
   glfwMakeContextCurrent( Window );
   glewInit();
   
   registerCallbacks();
   
   glEnable( GL_DEPTH_TEST );
   glClearColor( 0.2f, 0.2f, 0.2f, 1.0f );

   MainCamera->updateWindowSize( width, height );
   ObjectShader->setShader(
      "Shaders/VertexShaderForObject.glsl",
      "Shaders/FragmentShaderForObject.glsl"
   );
   ObjectShader->setComputeShaders( { 
      "Shaders/ComputeShaderForCloth.glsl"
   } );
}

void RendererGL::cleanup(GLFWwindow* window)
{
   glfwSetWindowShouldClose( window, GLFW_TRUE );
}

void RendererGL::cleanupWrapper(GLFWwindow* window)
{
   Renderer->cleanup( window );
}

void RendererGL::keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
   if (action != GLFW_PRESS) return;

   switch (key) {
      case GLFW_KEY_UP:
         MainCamera->moveForward();
         break;
      case GLFW_KEY_DOWN:
         MainCamera->moveBackward();
         break;
      case GLFW_KEY_LEFT:
         MainCamera->moveLeft();
         break;
      case GLFW_KEY_RIGHT:
         MainCamera->moveRight();
         break;
      case GLFW_KEY_W:
         MainCamera->moveUp();
         break;
      case GLFW_KEY_S:
         MainCamera->moveDown();
         break;
      case GLFW_KEY_I:
         MainCamera->resetCamera();
         break;
      case GLFW_KEY_L:
         Lights->toggleLightSwitch();
         cout << "Light Turned " << (Lights->isLightOn() ? "On!" : "Off!") << endl;
         break;
      case GLFW_KEY_Q:
      case GLFW_KEY_ESCAPE:
         cleanupWrapper( window );
         break;
      default:
         return;
   }
}

void RendererGL::keyboardWrapper(GLFWwindow* window, int key, int scancode, int action, int mods)
{
   Renderer->keyboard( window, key, scancode, action, mods );
}

void RendererGL::cursor(GLFWwindow* window, double xpos, double ypos)
{
   if (MainCamera->getMovingState()) {
      const auto x = static_cast<float>(round( xpos ));
      const auto y = static_cast<float>(round( ypos ));
      const int dx = static_cast<int>(x - ClickedPoint.x);
      const int dy = static_cast<int>(y - ClickedPoint.y);
      MainCamera->moveForward( -dy );
      MainCamera->rotateAroundWorldY( -dx );

      if (glfwGetMouseButton( window, GLFW_MOUSE_BUTTON_RIGHT ) == GLFW_PRESS) {
         MainCamera->pitch( -dy );
      }

      ClickedPoint.x = x;
      ClickedPoint.y = y;
   }
}

void RendererGL::cursorWrapper(GLFWwindow* window, double xpos, double ypos)
{
   Renderer->cursor( window, xpos, ypos );
}

void RendererGL::mouse(GLFWwindow* window, int button, int action, int mods)
{
   if (button == GLFW_MOUSE_BUTTON_LEFT) {
      const bool moving_state = action == GLFW_PRESS;
      if (moving_state) {
         double x, y;
         glfwGetCursorPos( window, &x, &y );
         ClickedPoint.x = static_cast<float>(round( x ));
         ClickedPoint.y = static_cast<float>(round( y ));
      }
      MainCamera->setMovingState( moving_state );
   }
}

void RendererGL::mouseWrapper(GLFWwindow* window, int button, int action, int mods)
{
   Renderer->mouse( window, button, action, mods );
}

void RendererGL::mousewheel(GLFWwindow* window, double xoffset, double yoffset)
{
   if (yoffset >= 0.0) MainCamera->zoomIn();
   else MainCamera->zoomOut();
}

void RendererGL::mousewheelWrapper(GLFWwindow* window, double xoffset, double yoffset)
{
   Renderer->mousewheel( window, xoffset, yoffset );
}

void RendererGL::reshape(GLFWwindow* window, int width, int height)
{
   MainCamera->updateWindowSize( width, height );
   glViewport( 0, 0, width, height );
}

void RendererGL::reshapeWrapper(GLFWwindow* window, int width, int height)
{
   Renderer->reshape( window, width, height );
}

void RendererGL::registerCallbacks() const
{
   glfwSetWindowCloseCallback( Window, cleanupWrapper );
   glfwSetKeyCallback( Window, keyboardWrapper );
   glfwSetCursorPosCallback( Window, cursorWrapper );
   glfwSetMouseButtonCallback( Window, mouseWrapper );
   glfwSetScrollCallback( Window, mousewheelWrapper );
   glfwSetFramebufferSizeCallback( Window, reshapeWrapper );
}

void RendererGL::setLights()
{  
   const vec4 light_position(100.0f, 500.0f, 100.0f, 1.0f);
   const vec4 ambient_color(1.0f, 1.0f, 1.0f, 1.0f);
   const vec4 diffuse_color(0.7f, 0.7f, 0.7f, 1.0f);
   const vec4 specular_color(0.9f, 0.9f, 0.9f, 1.0f);
   Lights->addLight( light_position, ambient_color, diffuse_color, specular_color );
}

void RendererGL::setClothObject() const
{
   const float ds = 1.0f / static_cast<float>(ClothPointNumSize.x - 1);
   const float dt = 1.0f / static_cast<float>(ClothPointNumSize.y - 1);
   const float dx = static_cast<float>(ClothGridSize.x) * ds;
   const float dy = static_cast<float>(ClothGridSize.y) * dt;
   
   vector<vec3> cloth_vertices, cloth_normals;
   vector<vec2> cloth_textures;
   for (int j = 0; j < ClothPointNumSize.y; ++j) {
      const auto y = static_cast<float>(j);
      for (int i = 0; i < ClothPointNumSize.x; ++i) {
         const auto x = static_cast<float>(i);   
         cloth_vertices.emplace_back( x * dx, 0.0f, y * dy );
         cloth_normals.emplace_back( 0.0f, 1.0f, 0.0f );
         cloth_textures.emplace_back( x * ds, y * dt );
      }
   }

   vector<GLuint> indices;
   for (int j = 0; j < ClothPointNumSize.y - 1; ++j) {
      for (int i = 0; i < ClothPointNumSize.x; ++i) {
         indices.emplace_back( (j + 1) * ClothPointNumSize.x + i );
         indices.emplace_back( j * ClothPointNumSize.x + i );
      }
   }

   ClothObject->setObject( GL_TRIANGLE_STRIP, cloth_vertices, cloth_normals, cloth_textures, "Samples/cloth.jpg" );
   ClothObject->setElementBuffer( indices );
   ClothObject->setDiffuseReflectionColor( { 1.0f, 1.0f, 1.0f, 1.0f } );
   ClothObject->prepareShaderStorageBuffer();
}

void RendererGL::setSphereObject() const
{
   SphereObject->setObject( GL_TRIANGLES, "Samples/sphere.obj", "Samples/sphere.jpg" );
   SphereObject->setDiffuseReflectionColor( { 1.0f, 1.0f, 1.0f, 1.0f } );
}

void RendererGL::setSuzanneObject() const
{
   SuzanneObject->setObject( GL_TRIANGLES, "Samples/suzanne.obj", "Samples/suzanne.jpg" );
   SuzanneObject->setDiffuseReflectionColor( { 1.0f, 1.0f, 1.0f, 1.0f } );
}

void RendererGL::setClothPhysicsVariables() const
{
   ObjectShader->addUniformLocation( ObjectShader->ComputeShaderPrograms[0], "SpringRestLength" );
   ObjectShader->addUniformLocation( ObjectShader->ComputeShaderPrograms[0], "SpringStiffness" );
   ObjectShader->addUniformLocation( ObjectShader->ComputeShaderPrograms[0], "SpringDamping" );
   ObjectShader->addUniformLocation( ObjectShader->ComputeShaderPrograms[0], "ShearRestLength" );
   ObjectShader->addUniformLocation( ObjectShader->ComputeShaderPrograms[0], "ShearStiffness" );
   ObjectShader->addUniformLocation( ObjectShader->ComputeShaderPrograms[0], "ShearDamping" );
   ObjectShader->addUniformLocation( ObjectShader->ComputeShaderPrograms[0], "BendingRestLength" );
   ObjectShader->addUniformLocation( ObjectShader->ComputeShaderPrograms[0], "BendingStiffness" );
   ObjectShader->addUniformLocation( ObjectShader->ComputeShaderPrograms[0], "BendingDamping" );
   ObjectShader->addUniformLocation( ObjectShader->ComputeShaderPrograms[0], "GravityConstant" );
   ObjectShader->addUniformLocation( ObjectShader->ComputeShaderPrograms[0], "GravityDamping" );
   ObjectShader->addUniformLocation( ObjectShader->ComputeShaderPrograms[0], "dt" );
   ObjectShader->addUniformLocation( ObjectShader->ComputeShaderPrograms[0], "Mass" );
   ObjectShader->addUniformLocation( ObjectShader->ComputeShaderPrograms[0], "ClothWorldMatrix" );
   ObjectShader->addUniformLocation( ObjectShader->ComputeShaderPrograms[0], "SpherePosition" );
   ObjectShader->addUniformLocation( ObjectShader->ComputeShaderPrograms[0], "SphereRadius" );
   ObjectShader->addUniformLocation( ObjectShader->ComputeShaderPrograms[0], "SphereWorldMatrix" );
}

void RendererGL::applyForces()
{
   const float rest_length = static_cast<float>(ClothGridSize.x) / static_cast<float>(ClothPointNumSize.x);
   glUseProgram( ObjectShader->ComputeShaderPrograms[0] );
   glUniform1f( ObjectShader->CustomLocations["SpringRestLength"], rest_length );
   glUniform1f( ObjectShader->CustomLocations["SpringStiffness"], 30.0f );
   glUniform1f( ObjectShader->CustomLocations["SpringDamping"], -0.5f );
   glUniform1f( ObjectShader->CustomLocations["ShearRestLength"], sqrt( 2.0f ) * rest_length );
   glUniform1f( ObjectShader->CustomLocations["ShearStiffness"], 30.0f );
   glUniform1f( ObjectShader->CustomLocations["ShearDamping"], -0.5f );
   glUniform1f( ObjectShader->CustomLocations["BendingRestLength"], 2.0f * rest_length );
   glUniform1f( ObjectShader->CustomLocations["BendingStiffness"], 15.0f );
   glUniform1f( ObjectShader->CustomLocations["BendingDamping"], -0.5f );
   glUniform1f( ObjectShader->CustomLocations["GravityConstant"], -5.0f );
   glUniform1f( ObjectShader->CustomLocations["GravityDamping"], -0.3f );
   glUniform1f( ObjectShader->CustomLocations["dt"], 0.1f );
   glUniform1f( ObjectShader->CustomLocations["Mass"], 1.0f );
   glUniformMatrix4fv( ObjectShader->CustomLocations["ClothWorldMatrix"], 1, GL_FALSE, &ClothWorldMatrix[0][0] );
   glUniform3fv( ObjectShader->CustomLocations["SpherePosition"], 1, &SpherePosition[0] );
   glUniform1f( ObjectShader->CustomLocations["SphereRadius"], SphereRadius );
   glUniformMatrix4fv( ObjectShader->CustomLocations["SphereWorldMatrix"], 1, GL_FALSE, &SphereWorldMatrix[0][0] );
   
   glDispatchCompute( ClothPointNumSize.x / 10, ClothPointNumSize.y / 10, 1 );
   glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );
   
   glBindBufferBase( GL_SHADER_STORAGE_BUFFER, 0, ClothObject->ShaderStorageBufferObjects[ClothTargetIndex] );
   glBindBufferBase( GL_SHADER_STORAGE_BUFFER, 1, ClothObject->ShaderStorageBufferObjects[(ClothTargetIndex + 1) % 3] );
   glBindBufferBase( GL_SHADER_STORAGE_BUFFER, 2, ClothObject->ShaderStorageBufferObjects[(ClothTargetIndex + 2) % 3] );
   ClothTargetIndex = (ClothTargetIndex + 1) % 3;
}

void RendererGL::drawClothObject(ShaderGL* shader, CameraGL* camera)
{
   const mat4 model_view_projection = camera->ProjectionMatrix * camera->ViewMatrix * ClothWorldMatrix;
   glUniformMatrix4fv( shader->Location.World, 1, GL_FALSE, &ClothWorldMatrix[0][0] );
   glUniformMatrix4fv( shader->Location.View, 1, GL_FALSE, &camera->ViewMatrix[0][0] );
   glUniformMatrix4fv( shader->Location.Projection, 1, GL_FALSE, &camera->ProjectionMatrix[0][0] );
   glUniformMatrix4fv( shader->Location.ModelViewProjection, 1, GL_FALSE, &model_view_projection[0][0] );
   
   ClothObject->transferUniformsToShader( shader );

   glBindTextureUnit( shader->Location.Texture[0].first, ClothObject->TextureID[0] );
   glBindVertexArray( ClothObject->ObjVAO );
   for (int j = 0; j < ClothPointNumSize.y - 1; ++j) {
      glDrawElements( 
         ClothObject->DrawMode, 
         ClothPointNumSize.x * 2, 
         GL_UNSIGNED_INT, 
         reinterpret_cast<GLvoid *>(j * ClothPointNumSize.x * 2 * sizeof GLuint)
      );
   }
}

void RendererGL::drawSphereObject(ShaderGL* shader, CameraGL* camera)
{
   const mat4 model_view_projection = camera->ProjectionMatrix * camera->ViewMatrix * SphereWorldMatrix;
   glUniformMatrix4fv( shader->Location.World, 1, GL_FALSE, &SphereWorldMatrix[0][0] );
   glUniformMatrix4fv( shader->Location.View, 1, GL_FALSE, &camera->ViewMatrix[0][0] );
   glUniformMatrix4fv( shader->Location.Projection, 1, GL_FALSE, &camera->ProjectionMatrix[0][0] );
   glUniformMatrix4fv( shader->Location.ModelViewProjection, 1, GL_FALSE, &model_view_projection[0][0] );

   glBindTextureUnit( shader->Location.Texture[0].first, SphereObject->TextureID[0] );
   glBindVertexArray( SphereObject->ObjVAO );
   glDrawArrays( SphereObject->DrawMode, 0, SphereObject->VerticesCount );
}

void RendererGL::drawSuzanneObject(ShaderGL* shader, CameraGL* camera)
{
   const mat4 to_world = 
      translate( mat4(1.0f), vec3(30.0f, 0.0f, 70.0f) ) *
      scale( mat4(1.0f), vec3( 20.0f, 20.0f, 20.0f ) );
   const mat4 model_view_projection = camera->ProjectionMatrix * camera->ViewMatrix * to_world;
   glUniformMatrix4fv( shader->Location.World, 1, GL_FALSE, &to_world[0][0] );
   glUniformMatrix4fv( shader->Location.View, 1, GL_FALSE, &camera->ViewMatrix[0][0] );
   glUniformMatrix4fv( shader->Location.Projection, 1, GL_FALSE, &camera->ProjectionMatrix[0][0] );
   glUniformMatrix4fv( shader->Location.ModelViewProjection, 1, GL_FALSE, &model_view_projection[0][0] );
   
   SuzanneObject->transferUniformsToShader( shader );

   glBindTextureUnit( shader->Location.Texture[0].first, SuzanneObject->TextureID[0] );
   glBindVertexArray( SuzanneObject->ObjVAO );
   glDrawArrays( SuzanneObject->DrawMode, 0, SuzanneObject->VerticesCount );
}

void RendererGL::render()
{
   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

   applyForces();

   glUseProgram( ObjectShader->ShaderProgram );
   Lights->transferUniformsToShader( ObjectShader.get() );
   drawClothObject( ObjectShader.get(), MainCamera.get() );
   drawSphereObject( ObjectShader.get(), MainCamera.get() );
   //drawSuzanneObject( ObjectShader.get(), MainCamera.get() );

   glBindVertexArray( 0 );
   glUseProgram( 0 );
}

void RendererGL::play()
{
   if (glfwWindowShouldClose( Window )) initialize();

   setLights();
   setClothObject();
   setSphereObject();
   //setSuzanneObject();
   setClothPhysicsVariables();
   ObjectShader->setUniformLocations( Lights->TotalLightNum );
   

   while (!glfwWindowShouldClose( Window )) {
      render();

      glfwSwapBuffers( Window );
      glfwPollEvents();
   }
   glfwDestroyWindow( Window );
}