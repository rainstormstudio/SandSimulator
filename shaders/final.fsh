#version 410

in vec2 texCoord;
out vec4 gl_FragColor;
uniform sampler2D theTexture;
uniform float uTime;

float clamp(float x, float min, float max) {
  if (x < min) {
    return min;
  } else if (x > max) {
    return max;
  } else {
    return x;
  }
}

float random(vec2 st) {
  return fract(sin(dot(st, vec2(12.9898, 78.233)) + uTime) * 43758.5453123);
}

float noise(vec2 st) {
  vec2 i = floor(st);
  vec2 f = fract(st);
  float a = random(i);
  float b = random(i + vec2(1.0, 0.0));
  float c = random(i + vec2(0.0, 1.0));
  float d = random(i + vec2(1.0, 1.0));
  vec2 u = f * f * (3.0 - 2.0 * f);
  u = smoothstep(0.0, 1.0, f);
  return mix(a, b, u.x) + (c - a) * u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
}

void main() {
  gl_FragColor = texture(theTexture, texCoord);
  if (gl_FragColor.r == 0.0f && gl_FragColor.g == 1.0f &&
      gl_FragColor.b == 1.0f) {
    vec2 st = texCoord;
    vec2 pos = vec2(st * 50.0);
    float n = noise(pos);
    n = smoothstep(0.5, 0.6, n);
    vec3 color = mix(vec3(1, 1, 1), vec3(0, 1, 1), n);
    
    pos = vec2(pos * 0.5);
    n = noise(pos);
    color = mix(color, vec3(0.5, 0.9, 0.9), n);

    gl_FragColor = vec4(color, 1.0);
  }
}