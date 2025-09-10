#version 110

const float PI = 3.1415926535897932384626433832795;
const float PI_2 = 1.57079632679489661923;

// Minecraft sky gradient
const vec4 color_bottom = vec4(0.722, 0.827, 1.0, 1.0);
const vec4 color_top = vec4(0.502, 0.678, 1.0, 1.0);

// values [-pi/2, pi/2]
uniform float u_camera_pitch;
uniform vec2 u_window_size;

void main()
{
    vec2 uv = gl_FragCoord.xy / u_window_size.xy;
    // values [-1, 1]
    float camera_face = u_camera_pitch / PI_2;
    float val = (7.0*uv.y - 3.0) - (4.0*camera_face) + (sin(uv.x * PI) / 3.0);
    vec4 color = mix(color_bottom, color_top, smoothstep(0.0, 1.0, val));
    gl_FragColor = color;
}
