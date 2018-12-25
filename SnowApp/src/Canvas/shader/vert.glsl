attribute float size;
attribute float rotation;
attribute vec4  color_;
attribute float textureIndex;

varying vec4  vColor;
varying float vRot;
varying float texIndex;

void main() {
  texIndex        = textureIndex;
  vColor          = color_;
  vRot            = rotation;
  vec4 mvPosition = modelViewMatrix * vec4( position, 1.0 );
  gl_PointSize    = size * ( 300.0 / -mvPosition.z );
  gl_Position     = projectionMatrix * mvPosition;
}
