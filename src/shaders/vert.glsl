#version 330 core

#define MAX_MESH_COUNT 8

// input locations of attributes
layout(location = 0) in vec3 vertCoordinates_in;
layout(location = 1) in vec3 vertNormal_in;
layout(location = 2) in vec2 vertUV_in;
layout(location = 3) in int vertMeshID_in;

// uniforms: matrices
uniform mat4 modelViewTransforms[MAX_MESH_COUNT];
uniform mat4 projectionTransform;
uniform mat3 normalTransforms[MAX_MESH_COUNT];

// output:
out vec3 fragViewSpacePos;
out vec3 fragNormal;
out vec2 fragUV;

void main() {
  vec4 viewSpacePos = modelViewTransforms[vertMeshID_in] * vec4(vertCoordinates_in, 1.0);

  // projection to screen space
  gl_Position = projectionTransform * viewSpacePos;

  fragViewSpacePos = viewSpacePos.xyz;
  fragNormal = normalize(normalTransforms[vertMeshID_in] * vertNormal_in);
  fragUV = vertUV_in;
}