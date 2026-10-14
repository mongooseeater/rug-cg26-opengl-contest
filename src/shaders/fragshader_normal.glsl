#version 330 core

// inputs to the fragment shader
in vec3 vertNormal;

// output of the fragment shader
out vec4 fColor;

void main() {
  fColor = vec4(0.5F * normalize(vertNormal) + vec3(0.5F, 0.5F, 0.5F), 1.0F);
}
