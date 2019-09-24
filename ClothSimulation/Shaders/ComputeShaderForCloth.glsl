#version 430

uniform float SpringRestLength;
uniform float SpringStiffness;
uniform float SpringDamping;
uniform float ShearRestLength;
uniform float ShearStiffness;
uniform float ShearDamping;
uniform float BendingRestLength;
uniform float BendingStiffness;
uniform float BendingDamping;

uniform float GravityConstant;
uniform float GravityDamping;

uniform float dt;
uniform float Mass;
uniform mat4 ClothWorldMatrix;

uniform vec3 SpherePosition;
uniform float SphereRadius;
uniform mat4 SphereWorldMatrix;

layout(local_size_x = 10, local_size_y = 10) in;

struct Attributes
{
   float x, y, z, nx, ny, nz, s, t;
};

layout(binding = 0, std430) buffer PrevPoints {
   Attributes Pn_prev[];
};

layout(binding = 1, std430) buffer CurrPoints {
   Attributes Pn[];
};

layout(binding = 2, std430) buffer NextPoints {
   Attributes Pn_next[];
};

struct Spring
{
   uint index; // 0xFFFFFFFF means that it is not the neighbor of the current vertex.
   float k;
   float rest_length;
   float damping;
};
Spring neighbors[12];

const float zero = 0.0f;
const float one = 1.0f;

vec4 Gravity = vec4(zero, GravityConstant, zero, zero);

void setNeighborSprings(uint index, uint cols, uint rows)
{
   neighbors[0].index = 0 < gl_GlobalInvocationID.y ? index - cols : 0xFFFFFFFF;                     // top
   neighbors[1].index = gl_GlobalInvocationID.y < rows - 1 ? index + cols : 0xFFFFFFFF;              // bottom
   neighbors[2].index = 0 < gl_GlobalInvocationID.x ? index - 1 : 0xFFFFFFFF;                        // left
   neighbors[3].index = gl_GlobalInvocationID.x < cols - 1 ? index + 1 : 0xFFFFFFFF;                 // right
   neighbors[4].index = neighbors[0].index != 0xFFFFFFFF && neighbors[2].index != 0xFFFFFFFF ? 
      neighbors[0].index - 1 : 0xFFFFFFFF;                                                           // top-left
   neighbors[5].index = neighbors[0].index != 0xFFFFFFFF && neighbors[3].index != 0xFFFFFFFF ? 
      neighbors[0].index + 1 : 0xFFFFFFFF;                                                           // top-right
   neighbors[6].index = neighbors[1].index != 0xFFFFFFFF && neighbors[2].index != 0xFFFFFFFF ? 
      neighbors[1].index - 1 : 0xFFFFFFFF;                                                           // bottom-left
   neighbors[7].index = neighbors[1].index != 0xFFFFFFFF && neighbors[3].index != 0xFFFFFFFF ? 
      neighbors[1].index + 1 : 0xFFFFFFFF;                                                           // bottom-right
   neighbors[8].index = 1 < gl_GlobalInvocationID.y ? neighbors[0].index - cols : 0xFFFFFFFF;        // top-top
   neighbors[9].index = gl_GlobalInvocationID.y < rows - 2 ? neighbors[1].index + cols : 0xFFFFFFFF; // bottom-bottom
   neighbors[10].index = 1 < gl_GlobalInvocationID.x ? neighbors[2].index - 1 : 0xFFFFFFFF;          // left-left
   neighbors[11].index = gl_GlobalInvocationID.x < cols - 2 ? neighbors[3].index + 1 : 0xFFFFFFFF;   // right-right

   for (int i = 0; i < 4; ++i) {
      neighbors[i].k = SpringStiffness;
      neighbors[i].rest_length = SpringRestLength;
      neighbors[i].damping = SpringDamping;
   }
   for (int i = 4; i < 8; ++i) {
      neighbors[i].k = ShearStiffness;
      neighbors[i].rest_length = SpringRestLength;
      neighbors[i].damping = ShearDamping;
   }
   for (int i = 8; i < 12; ++i) {
      neighbors[i].k = BendingStiffness;
      neighbors[i].rest_length = SpringRestLength * 2;
      neighbors[i].damping = BendingDamping;
   }
}

vec4 calculateMassSpringForce(vec4 p_curr, vec4 velocity)
{
   vec4 force = vec4(zero, zero, zero, zero);
   for (int i = 0; i < 12; ++i) {
      uint n = neighbors[i].index;
      if (n == 0xFFFFFFFF) continue;

      vec4 neighbor = vec4(Pn[n].x, Pn[n].y, Pn[n].z, one);
      vec4 neighbor_prev = vec4(Pn_prev[n].x, Pn_prev[n].y, Pn_prev[n].z, one);
      vec4 neighbor_velocity = (neighbor - neighbor_prev) / dt;
      vec4 dl = p_curr - neighbor;
      vec4 dv = velocity - neighbor_velocity;
      float l = length( dl );
      float spring_force = neighbors[i].k * (neighbors[i].rest_length - l);
      float damping_force = neighbors[i].damping * dot( dl, dv ) / l;
      force += (spring_force + damping_force) * normalize( dl );
   }
   return force;
}

vec4 calculateGravityForce(vec4 velocity)
{
   return Mass * Gravity + velocity * GravityDamping;
}

vec4 calculateFrictionOnSphere()
{
   vec4 sphereCenter = vec4(sphereX, 0, 0, 1);
   float currDist = length(current.xyz - sphereCenter.xyz);
   float radius = sqrt(0.5*0.5+structRest*structRest);
   //if (false)
   force += GRAVITY * mass + vel * GRAVITY_DAMPING;
   if (currDist <= radius+0.0005)
   {
      vec3 normal = normalize(current.xyz - sphereCenter.xyz);
      vec3 tangent = normalize(cross(cross(normal, force.xyz), normal));
      //수직항력
      float vertical = max(dot(force.xyz, -normal), 0);
      //수평성분 (force)
      float horizontal = max(dot(force.xyz, tangent), 0);
      //마찰계수

      if (vertical != 0)
      {
         //float kfr = 0.5;
         //마찰력0
         float friction = vertical * kfr;

         force = max(horizontal - friction, 0) * vec4(tangent, 0) + vel*-0.75;

         if (length(force) > 0 || vertical == 0)
            movable = true;
         else
         {
            movable = false;
         }
      }
   }
}

/*vec4 calculateWindForce(vec4 velocity)
{
   float random = fract( sin( dot( gl_GlobalInvocationID.xy ,vec2(12.9898f, 78.233f) ) ) * 43758.5453f );
   return random * windForce(normalize(vec4(normal, 1))) + velocity * DEFAULT_DAMPING;
}*/

vec3 update(vec4 force, vec4 p_curr, vec4 velocity, uint index)
{
   vec4 acceleration = force / Mass;
   vec4 updated = p_curr + velocity * dt + acceleration * dt * dt;
   return updated.xyz;
}

void detectCollisionWithSphere(inout vec3 updated, uint index)
{
   vec4 updated_in_wc = ClothWorldMatrix * vec4(updated, one);
   vec4 sphere_in_wc = SphereWorldMatrix * vec4(SpherePosition, one);
   vec3 d = (updated_in_wc - sphere_in_wc).xyz;
   float distance = length( d );
   if (distance < SphereRadius) {
      updated_in_wc.xyz += (SphereRadius - distance) * normalize( d );
      updated = vec3(inverse( ClothWorldMatrix ) * updated_in_wc);
   }
}

void detectCollisionWithFloor(inout vec3 updated, uint index)
{
   vec4 updated_in_wc = ClothWorldMatrix * vec4(updated, one);
   if (updated_in_wc.y < 0.0f) updated.y = Pn[index].y;
}

void main() 
{
   uvec3 points = gl_NumWorkGroups * gl_WorkGroupSize;
   uint index = gl_GlobalInvocationID.y * points.x + gl_GlobalInvocationID.x;

   vec4 p_curr = vec4(Pn[index].x, Pn[index].y, Pn[index].z, one);
   vec4 p_prev = vec4(Pn_prev[index].x, Pn_prev[index].y, Pn_prev[index].z, one);
   vec4 velocity = (p_curr - p_prev) / dt;

   setNeighborSprings( index, points.x, points.y );

   vec4 force = calculateMassSpringForce( p_curr, velocity );
   force += calculateGravityForce( velocity );
   force += calculateFrictionOnSphere();
   // force += calculateWindForce();

   vec3 updated = update( force, p_curr, velocity, index );

   // ... adjust ...

   detectCollisionWithSphere( updated, index );

   detectCollisionWithFloor( updated, index );

   Pn_next[index].x = updated.x;
   Pn_next[index].y = updated.y;
   Pn_next[index].z = updated.z;
   Pn_next[index].s = Pn[index].s;
   Pn_next[index].t = Pn[index].t;
}