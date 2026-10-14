#version 330 core

// input locations of attributes
layout(location = 0) in vec3 vertCoordinates_in;
layout(location = 1) in vec3 vertNormal_in;

// uniforms: matrices
uniform mat4 modelViewTransform;
uniform mat4 projectionTransform;
uniform mat3 normalTransform;

// uniforms: material
uniform vec3 matCol;        // (local) material colour
uniform float matKA;        // ambient coefficient
uniform float matKD;        // diffuse coefficient
uniform float matKS;        // specular coefficient
uniform float matExponent;  // specular exponent

// uniforms: light
uniform vec3 lightPos;      // light position
uniform vec3 lightCol;      // light colour

// shader output: vertex colour
out vec3 vertCol;

// we transform everything to view/world space (these are the same, given the camera stays fixed)
void main() {
  // we always keep the camera at (0,0,0)
  vec3 viewPos = vec3(0.0, 0.0, 0.0);

  vec3 n = normalize(normalTransform * vertNormal_in);
  vec4 viewSpacePos = modelViewTransform * vec4(vertCoordinates_in, 1.0);

  // unit direction vectors towards light and view,
  // and in direction of reflected ("specular") light
  vec3 l = normalize(lightPos - viewSpacePos.xyz);
  vec3 v = normalize(viewPos - viewSpacePos.xyz);
  vec3 r = reflect(-l, n);

  // max(0, dot_product) -> ensure light and view on same side of surface
  float dot_nl = max(dot(n, l), 0.0);
  float dot_rv = max(dot(r, v), 0.0);

  // a/d/s illumination respectively:
  vec3 i_ambient = matKA * matCol;
  vec3 i_diffuse = lightCol * matCol * matKD * dot_nl;
  vec3 i_specular = lightCol * matKS * pow(dot_rv, matExponent);

  // projection to screen space
  gl_Position = projectionTransform * viewSpacePos;

  // output: (phong) illumination of vertex
  vertCol = i_ambient + i_diffuse + i_specular;
}