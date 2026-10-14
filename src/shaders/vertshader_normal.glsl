#version 330 core

// input locations of attributes
layout(location = 0) in vec3 vertCoordinates_in;
layout(location = 1) in vec3 vertNormal_in;

// uniforms: matrices
uniform mat4 modelViewTransform;
uniform mat4 projectionTransform;
uniform mat3 normalTransform;

// output of the vertex stage
out vec3 vertNormal;

void main() {
  // projection to screen space     (gl_Position is an output (a vec4) of the vertex shader)
  gl_Position = projectionTransform * modelViewTransform * vec4(vertCoordinates_in, 1.0F);
  vertNormal = normalize(normalTransform * vertNormal_in);
}
