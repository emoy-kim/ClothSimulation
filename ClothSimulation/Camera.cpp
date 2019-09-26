#include "Camera.h"

CameraGL::CameraGL() : 
   CameraGL(vec3(80.0f, 50.0f, 300.0f), vec3(80.0f, 50.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f))
{   
}

CameraGL::CameraGL(
   const vec3& cam_position,
   const vec3& view_reference_position,
   const vec3& view_up_vector,
   float fov,
   float near_plane,
   float far_plane
) : 
   ZoomSensitivity( 1.0f ), MoveSensitivity( 5.0f ), RotationSensitivity( 0.005f ), IsMoving( false ),
   Width( 0 ), Height( 0 ), AspectRatio( 0.0f ), InitFOV( fov ), NearPlane( near_plane ), FarPlane( far_plane ), 
   InitCamPos( cam_position ), InitRefPos( view_reference_position ), InitUpVec( view_up_vector ), 
   FOV( fov ), CamPos( cam_position ),
   ViewMatrix( lookAt( InitCamPos, InitRefPos, InitUpVec ) ), ProjectionMatrix( mat4(1.0f) )
{
}

bool CameraGL::getMovingState() const
{
   return IsMoving;
}

int CameraGL::getCameraWidth() const
{
   return Width;
}

int CameraGL::getCameraHeight() const
{
   return Height;
}

float CameraGL::getCameraNearPlane() const
{
   return NearPlane;
}
   
float CameraGL::getCameraFarPlane() const
{
   return FarPlane;
}

void CameraGL::setMovingState(const bool& is_moving)
{
   IsMoving = is_moving;
}

void CameraGL::updateCamera()
{
   const mat4 inverse_view = inverse( ViewMatrix );
   CamPos.x = inverse_view[3][0];
   CamPos.y = inverse_view[3][1];
   CamPos.z = inverse_view[3][2];
}

void CameraGL::pitch(const int& angle)
{
   const vec3 u_axis(ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0]);
   ViewMatrix = glm::rotate( ViewMatrix, static_cast<float>(-angle) * RotationSensitivity, u_axis );
   updateCamera();
}

void CameraGL::yaw(const int& angle)
{
   const vec3 v_axis(ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]);
   ViewMatrix = glm::rotate( ViewMatrix, static_cast<float>(-angle) * RotationSensitivity, v_axis );
   updateCamera();
}

void CameraGL::rotateAroundWorldY(const int& angle)
{
   const vec3 world_y(0.0f, 1.0f, 0.0f);
   ViewMatrix = glm::rotate( mat4(1.0f), static_cast<float>(-angle) * RotationSensitivity, world_y ) * ViewMatrix;
   updateCamera();
}

void CameraGL::moveForward(const int& delta)
{
   const vec3 n_axis(ViewMatrix[0][2], ViewMatrix[1][2], ViewMatrix[2][2]);
   ViewMatrix = translate( ViewMatrix, MoveSensitivity * static_cast<float>(-delta) *  -n_axis );
   updateCamera();
}

void CameraGL::moveBackward(const int& delta)
{
   const vec3 n_axis(ViewMatrix[0][2], ViewMatrix[1][2], ViewMatrix[2][2]);
   ViewMatrix = translate( ViewMatrix, MoveSensitivity * static_cast<float>(-delta) * n_axis );
   updateCamera();
}

void CameraGL::moveLeft(const int& delta)
{
   const vec3 u_axis(ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0]);
   ViewMatrix = translate( ViewMatrix, MoveSensitivity * static_cast<float>(-delta) * -u_axis );
   updateCamera();
}

void CameraGL::moveRight(const int& delta)
{
   const vec3 u_axis(ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0]);
   ViewMatrix = translate( ViewMatrix, MoveSensitivity * static_cast<float>(-delta) * u_axis );
   updateCamera();
}

void CameraGL::moveUp(const int& delta)
{
   const vec3 v_axis(ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]);
   ViewMatrix = translate( ViewMatrix, MoveSensitivity * static_cast<float>(-delta) * v_axis );
   updateCamera();
}

void CameraGL::moveDown(const int& delta)
{
   const vec3 v_axis(ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]);
   ViewMatrix = translate( ViewMatrix, MoveSensitivity * static_cast<float>(-delta) * -v_axis );
   updateCamera();
}

void CameraGL::zoomIn()
{
   if (FOV > 0.0f) {
      FOV -= ZoomSensitivity;
      ProjectionMatrix = perspective( radians( FOV ), AspectRatio, NearPlane, FarPlane );
   }
}

void CameraGL::zoomOut()
{
   if (FOV < 90.0f) {
      FOV += ZoomSensitivity;
      ProjectionMatrix = perspective( radians( FOV ), AspectRatio, NearPlane, FarPlane );
   }
}

void CameraGL::resetCamera()
{
   CamPos = InitCamPos; 
   ViewMatrix = lookAt( InitCamPos, InitRefPos, InitUpVec );
   ProjectionMatrix = perspective( radians( InitFOV ), AspectRatio, NearPlane, FarPlane );
}

void CameraGL::updateWindowSize(const int& width, const int& height)
{
   Width = width;
   Height = height;
   AspectRatio = static_cast<float>(width) / static_cast<float>(height);
   ProjectionMatrix = perspective( radians( FOV ), AspectRatio, NearPlane, FarPlane );
}

void CameraGL::updateCameraPosition(
   const vec3& cam_position,
   const vec3& view_reference_position,
   const vec3& view_up_vector
)
{
   InitCamPos = cam_position; 
   InitRefPos = view_reference_position; 
   InitUpVec = view_up_vector;
   ViewMatrix = lookAt( InitCamPos, InitRefPos, InitUpVec );
   updateCamera();
}

void CameraGL::updateNearFarPlanes(const float& near_plane, const float& far_plane)
{
   NearPlane = near_plane;
   FarPlane = far_plane;
   ProjectionMatrix = perspective( radians( FOV ), AspectRatio, NearPlane, FarPlane );
}