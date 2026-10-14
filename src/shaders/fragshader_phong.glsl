#version 330 core

// inputs to the fragment shader
in vec3 fragViewSpacePos;
in vec3 fragNormal;
in vec2 fragUV;

// uniforms: material
uniform float matKA;        // ambient coefficient
uniform float matKD;        // diffuse coefficient
uniform float matKS;        // specular coefficient
uniform float matExponent;  // specular exponent

// uniforms: light
uniform vec3 lightPos;      // light position
uniform vec3 lightCol;      // light colour

// uniforms: texture/sampling
uniform sampler2D texSampler;

// output of the fragment shader
out vec4 fColor;

// everything in view/world space (these are the same, given the camera stays fixed)
void main() {
  vec3 viewPos = vec3(0.0, 0.0, 0.0);

  // sample texture at this vertex
  vec3 texCol = texture(texSampler, fragUV).rgb;

  // required vectors
  vec3 p = fragViewSpacePos;
  vec3 n = normalize(fragNormal);
  vec3 l = normalize(lightPos - p);
  vec3 v = normalize(viewPos - p);
  vec3 r = reflect(-l, n);

  // max(0, dot_product) -> ensure light and view on same side of surface
  float dot_nl = max(dot(n, l), 0.0);
  float dot_rv = max(dot(r, v), 0.0);

  // a/d/s illumination respectively:
  vec3 i_ambient = matKA * texCol;
  vec3 i_diffuse = lightCol * texCol * matKD * dot_nl;
  vec3 i_specular = lightCol * matKS * pow(dot_rv, matExponent);

  fColor = vec4(i_ambient + i_diffuse + i_specular, 1.0F);
}
