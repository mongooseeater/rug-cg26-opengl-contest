#version 330 core

// inputs to the fragment shader
in vec3 vertCol;

// output of the fragment shader
out vec4 fColor;

void main() {
  // interpolate vertex colours over face (done by opengl)
  fColor = vec4(vertCol, 1.0F);
}
