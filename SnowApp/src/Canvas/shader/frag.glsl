// rotation shader from openFrameworks/examples/gl/billboardRotationExample
uniform sampler2D tex00;
uniform sampler2D tex01;
uniform sampler2D tex02;
uniform sampler2D tex03;
uniform sampler2D tex04;
uniform sampler2D tex05;
uniform sampler2D tex06;

varying vec4 vColor;
varying float vRot;
varying float texIndex;

void main() {
  float angle = vRot;
  vec2 offset = vec2(.5, .5);
  vec2 v2     = gl_PointCoord.st;

  // not sure this is right
  // I just search on how to rotate
  // a texture...
  float cosX, sinX;
  float cosY, sinY;
  sinX = sin(angle);
  cosX = cos(angle);
  sinY = sin(angle);
  cosY = cos(angle);

  mat2 rotationMatrix = mat2( cosX, -sinX,
                              sinY,  cosX );

  vec2 newcoords = ((v2 - offset) * (rotationMatrix));
  newcoords     += offset;

  vec4 color = texture2D(tex00, newcoords) * vColor;

  if(texIndex == 0.0) color = texture2D(tex00, newcoords) * vColor;
  if(texIndex == 1.0) color = texture2D(tex01, newcoords) * vColor;
  if(texIndex == 2.0) color = texture2D(tex02, newcoords) * vColor;
  if(texIndex == 3.0) color = texture2D(tex03, newcoords) * vColor;
  if(texIndex == 4.0) color = texture2D(tex04, newcoords) * vColor;
  if(texIndex == 5.0) color = texture2D(tex05, newcoords) * vColor;
  if(texIndex == 6.0) color = texture2D(tex06, newcoords) * vColor;

  gl_FragColor   = color;
}
