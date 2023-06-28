#include "Renderer.h"

RendererGL::RendererGL() : 
   Window( nullptr ), FrameWidth( 1920 ), FrameHeight( 1080 ), ClickedPoint( -1, -1 ),
   ClothTargetIndex( 0 ), ClothPointNumSize( 100, 100 ), ClothGridSize( 50, 50 ),
   SpherePosition( 0.0f, 0.0f, 0.0f ), SphereRadius( 20.0f ),
   ClothWorldMatrix( translate( glm::mat4(1.0f), glm::vec3(50.0f, 100.0f, 0.0f) ) ),
   SphereWorldMatrix( translate( glm::mat4(1.0f), glm::vec3(100.0f, 30.0f, 20.0f) ) ),
   MainCamera( std::make_unique<CameraGL>() ), ObjectShader( std::make_unique<ShaderGL>() ),
   ClothObject( std::make_unique<ObjectGL>() ), SphereObject( std::make_unique<ObjectGL>() ),
   Lights( std::make_unique<LightGL>() )
{
   Renderer = this;

   initialize();
   printOpenGLInformation();
}

void RendererGL::printOpenGLInformation()
{
   std::cout << "****************************************************************\n";
   std::cout << " - GLFW version supported: " << glfwGetVersionString() << "\n";
   std::cout << " - OpenGL renderer: " << glGetString( GL_RENDERER ) << "\n";
   std::cout << " - OpenGL version supported: " << glGetString( GL_VERSION ) << "\n";
   std::cout << " - OpenGL shader version supported: " << glGetString( GL_SHADING_LANGUAGE_VERSION ) << "\n";
   std::cout << "****************************************************************\n\n";
}

void RendererGL::initialize()
{
   if (!glfwInit()) {
      std::cout << "Cannot Initialize OpenGL...\n";
      return;
   }
   glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
   glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 6 );
   glfwWindowHint( GLFW_DOUBLEBUFFER, GLFW_TRUE );
   glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

   Window = glfwCreateWindow( FrameWidth, FrameHeight, "Main Camera", nullptr, nullptr );
   glfwMakeContextCurrent( Window );

   if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
      std::cout << "Failed to initialize GLAD" << std::endl;
      return;
   }
   
   registerCallbacks();
   
   glEnable( GL_DEPTH_TEST );
   glClearColor( 0.3f, 0.3f, 0.3f, 1.0f );

   MainCamera->updateWindowSize( FrameWidth, FrameHeight );

   const std::string shader_directory_path = std::string(CMAKE_SOURCE_DIR) + "/shaders";
   ObjectShader->setShader(
      std::string(shader_directory_path + "/BasicPipeline.vert").c_str(),
      std::string(shader_directory_path + "/BasicPipeline.frag").c_str()
   );
   ObjectShader->setComputeShaders( { 
      std::string(shader_directory_path + "/ClothSimulator.comp").c_str()
   } );
}

void RendererGL::error(int error, const char* description) const
{
   puts( description );
}

void RendererGL::errorWrapper(int error, const char* description)
{
   Renderer->error( error, description );
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
         std::cout << "Light Turned " << (Lights->isLightOn() ? "On!\n" : "Off!\n");
         break;
      case GLFW_KEY_P: {
         const glm::vec3 pos = MainCamera->getCameraPosition();
         std::cout << "Camera Position: " << pos.x << ", " << pos.y << ", " << pos.z << "\n";
      } break;
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
      const auto x = static_cast<int>(round( xpos ));
      const auto y = static_cast<int>(round( ypos ));
      const int dx = x - ClickedPoint.x;
      const int dy = y - ClickedPoint.y;
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
         ClickedPoint.x = static_cast<int>(round( x ));
         ClickedPoint.y = static_cast<int>(round( y ));
      }
      MainCamera->setMovingState( moving_state );
   }
}

void RendererGL::mouseWrapper(GLFWwindow* window, int button, int action, int mods)
{
   Renderer->mouse( window, button, action, mods );
}

void RendererGL::mousewheel(GLFWwindow* window, double xoffset, double yoffset) const
{
   if (yoffset >= 0.0) MainCamera->zoomIn();
   else MainCamera->zoomOut();
}

void RendererGL::mousewheelWrapper(GLFWwindow* window, double xoffset, double yoffset)
{
   Renderer->mousewheel( window, xoffset, yoffset );
}

void RendererGL::reshape(GLFWwindow* window, int width, int height) const
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
   glfwSetErrorCallback( errorWrapper );
   glfwSetWindowCloseCallback( Window, cleanupWrapper );
   glfwSetKeyCallback( Window, keyboardWrapper );
   glfwSetCursorPosCallback( Window, cursorWrapper );
   glfwSetMouseButtonCallback( Window, mouseWrapper );
   glfwSetScrollCallback( Window, mousewheelWrapper );
   glfwSetFramebufferSizeCallback( Window, reshapeWrapper );
}

void RendererGL::setLights() const
{  
   const glm::vec4 light_position(30.0f, 500.0f, 30.0f, 1.0f);
   const glm::vec4 ambient_color(1.0f, 1.0f, 1.0f, 1.0f);
   const glm::vec4 diffuse_color(0.7f, 0.7f, 0.7f, 1.0f);
   const glm::vec4 specular_color(0.9f, 0.9f, 0.9f, 1.0f);
   Lights->addLight( light_position, ambient_color, diffuse_color, specular_color );
}

void RendererGL::setClothObject() const
{
   const float ds = 1.0f / static_cast<float>(ClothPointNumSize.x - 1);
   const float dt = 1.0f / static_cast<float>(ClothPointNumSize.y - 1);
   const float dx = static_cast<float>(ClothGridSize.x) * ds;
   const float dy = static_cast<float>(ClothGridSize.y) * dt;
   
   std::vector<glm::vec3> cloth_vertices, cloth_normals;
   std::vector<glm::vec2> cloth_textures;
   for (int j = 0; j < ClothPointNumSize.y; ++j) {
      const auto y = static_cast<float>(j);
      for (int i = 0; i < ClothPointNumSize.x; ++i) {
         const auto x = static_cast<float>(i);   
         cloth_vertices.emplace_back( x * dx, 0.0f, y * dy );
         cloth_normals.emplace_back( 0.0f, 1.0f, 0.0f );
         cloth_textures.emplace_back( x * ds, y * dt );
      }
   }

   std::vector<GLuint> indices;
   for (int j = 0; j < ClothPointNumSize.y - 1; ++j) {
      for (int i = 0; i < ClothPointNumSize.x; ++i) {
         indices.emplace_back( (j + 1) * ClothPointNumSize.x + i );
         indices.emplace_back( j * ClothPointNumSize.x + i );
      }
   }

   const std::string sample_directory_path = std::string(CMAKE_SOURCE_DIR) + "/samples";
   ClothObject->setObject( 
      GL_TRIANGLE_STRIP, 
      cloth_vertices, 
      cloth_normals, 
      cloth_textures, 
      std::string(sample_directory_path + "/cloth.jpg") 
   );
   ClothObject->setElementBuffer( indices );
   ClothObject->setDiffuseReflectionColor( { 1.0f, 1.0f, 1.0f, 1.0f } );
   ClothObject->prepareShaderStorageBuffer();
}

void RendererGL::setSphereObject() const
{
   const std::string sample_directory_path = std::string(CMAKE_SOURCE_DIR) + "/samples";
   SphereObject->setObject( 
      GL_TRIANGLES, 
      std::string(sample_directory_path + "/sphere.obj"), 
      std::string(sample_directory_path + "/sphere.jpg") 
   );
   SphereObject->setDiffuseReflectionColor( { 1.0f, 1.0f, 1.0f, 1.0f } );
}

void RendererGL::setClothPhysicsVariables() const
{
   ObjectShader->addUniformLocationToComputeShader( "SpringRestLength", 0 );
   ObjectShader->addUniformLocationToComputeShader( "SpringStiffness", 0 );
   ObjectShader->addUniformLocationToComputeShader( "SpringDamping", 0 );
   ObjectShader->addUniformLocationToComputeShader( "ShearRestLength", 0 );
   ObjectShader->addUniformLocationToComputeShader( "ShearStiffness", 0 );
   ObjectShader->addUniformLocationToComputeShader( "ShearDamping", 0 );
   ObjectShader->addUniformLocationToComputeShader( "FlexionRestLength", 0 );
   ObjectShader->addUniformLocationToComputeShader( "FlexionStiffness", 0 );
   ObjectShader->addUniformLocationToComputeShader( "FlexionDamping", 0 );
   ObjectShader->addUniformLocationToComputeShader( "GravityConstant", 0 );
   ObjectShader->addUniformLocationToComputeShader( "GravityDamping", 0 );
   ObjectShader->addUniformLocationToComputeShader( "dt", 0 );
   ObjectShader->addUniformLocationToComputeShader( "Mass", 0 );
   ObjectShader->addUniformLocationToComputeShader( "ClothPosition", 0 );
   ObjectShader->addUniformLocationToComputeShader( "ClothWorldMatrix", 0 );
   ObjectShader->addUniformLocationToComputeShader( "SpherePosition", 0 );
   ObjectShader->addUniformLocationToComputeShader( "SphereRadius", 0 );
   ObjectShader->addUniformLocationToComputeShader( "SphereWorldMatrix", 0 );
}

void RendererGL::applyForces()
{
   const float rest_length = static_cast<float>(ClothGridSize.x) / static_cast<float>(ClothPointNumSize.x);
   glUseProgram( ObjectShader->getComputeShaderProgram( 0 ) );
   glUniform1f( ObjectShader->getLocation( "SpringRestLength" ), rest_length );
   glUniform1f( ObjectShader->getLocation( "SpringStiffness" ), 10.0f );
   glUniform1f( ObjectShader->getLocation( "SpringDamping" ), -0.5f );
   glUniform1f( ObjectShader->getLocation( "ShearRestLength" ), 1.5f * rest_length );
   glUniform1f( ObjectShader->getLocation( "ShearStiffness" ), 10.0f );
   glUniform1f( ObjectShader->getLocation( "ShearDamping" ), -0.5f );
   glUniform1f( ObjectShader->getLocation( "FlexionRestLength" ), 2.0f * rest_length );
   glUniform1f( ObjectShader->getLocation( "FlexionStiffness" ), 5.0f );
   glUniform1f( ObjectShader->getLocation( "FlexionDamping" ), -0.5f );
   glUniform1f( ObjectShader->getLocation( "GravityConstant" ), -5.0f );
   glUniform1f( ObjectShader->getLocation( "GravityDamping" ), -0.3f );
   glUniform1f( ObjectShader->getLocation( "dt" ), 0.1f );
   glUniform1f( ObjectShader->getLocation( "Mass" ), 1.0f );
   glUniformMatrix4fv( ObjectShader->getLocation( "ClothWorldMatrix" ), 1, GL_FALSE, &ClothWorldMatrix[0][0] );
   glUniform3fv( ObjectShader->getLocation( "SpherePosition" ), 1, &SpherePosition[0] );
   glUniform1f( ObjectShader->getLocation( "SphereRadius" ), SphereRadius );
   glUniformMatrix4fv( ObjectShader->getLocation( "SphereWorldMatrix" ), 1, GL_FALSE, &SphereWorldMatrix[0][0] );
   
   glDispatchCompute( ClothPointNumSize.x / 10, ClothPointNumSize.y / 10, 1 );
   glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );
   
   glBindBufferBase( GL_SHADER_STORAGE_BUFFER, 0, ClothObject->getShaderStorageBuffer( ClothTargetIndex ) );
   glBindBufferBase( GL_SHADER_STORAGE_BUFFER, 1, ClothObject->getShaderStorageBuffer( (ClothTargetIndex + 1) % 3 ) );
   glBindBufferBase( GL_SHADER_STORAGE_BUFFER, 2, ClothObject->getShaderStorageBuffer( (ClothTargetIndex + 2) % 3 ) );
   ClothTargetIndex = (ClothTargetIndex + 1) % 3;
}

void RendererGL::drawClothObject() const
{
   ObjectShader->transferBasicTransformationUniforms( ClothWorldMatrix, MainCamera.get(), true );
   ClothObject->transferUniformsToShader( ObjectShader.get() );

   glBindTextureUnit( 0, ClothObject->getTextureID( 0 ) );
   glBindVertexArray( ClothObject->getVAO() );
   for (int j = 0; j < ClothPointNumSize.y - 1; ++j) {
      glDrawElements( 
         ClothObject->getDrawMode(), 
         ClothPointNumSize.x * 2, 
         GL_UNSIGNED_INT, 
         reinterpret_cast<GLvoid*>(j * ClothPointNumSize.x * 2 * sizeof(GLuint))
      );
   }
}

void RendererGL::drawSphereObject() const
{
   const glm::mat4 to_world = SphereWorldMatrix * translate(glm::mat4(1.0f), SpherePosition );
   ObjectShader->transferBasicTransformationUniforms( to_world, MainCamera.get(), true );
   SphereObject->transferUniformsToShader( ObjectShader.get() );

   glBindTextureUnit( 0, SphereObject->getTextureID( 0 ) );
   glBindVertexArray( SphereObject->getVAO() );
   glDrawArrays( SphereObject->getDrawMode(), 0, SphereObject->getVertexNum() );
}

void RendererGL::render()
{
   glClear( OPENGL_COLOR_BUFFER_BIT | OPENGL_DEPTH_BUFFER_BIT );

   applyForces();

   MainCamera->updateWindowSize( FrameWidth, FrameHeight );
   glViewport( 0, 0, FrameWidth, FrameHeight );

   glUseProgram( ObjectShader->getShaderProgram() );
   Lights->transferUniformsToShader( ObjectShader.get() );
   drawClothObject();
   drawSphereObject();

   glBindVertexArray( 0 );
   glUseProgram( 0 );
}

void RendererGL::play()
{
   if (glfwWindowShouldClose( Window )) initialize();

   setLights();
   setClothObject();
   setSphereObject();
   setClothPhysicsVariables();
   ObjectShader->setUniformLocations( Lights->getTotalLightNum() );

   while (!glfwWindowShouldClose( Window )) {
      render();

      glfwSwapBuffers( Window );
      glfwPollEvents();
   }
   glfwDestroyWindow( Window );
}