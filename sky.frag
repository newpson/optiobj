#version 110

// Minecraft sky gradient
const vec4 color_bottom = vec4(0.722, 0.827, 1.0, 1.0);
const vec4 color_top = vec4(0.502, 0.678, 1.0, 1.0);

// values [0, 1] // TODO values [-pi/2, pi/2] and normalize
uniform float camera_pitch;
uniform vec2 window_size;

void main()
{
    vec2 uv = gl_FragCoord.xy / window_size.xy;
    // vales [-1, 1]
    float camera_face = (camera_pitch - 0.5) * 2.0;

    float val = (7.0*uv.y - 3.0) - (4.0*camera_face) + (sin(uv.x * 3.1415) / 3.0);
    vec4 color = mix(color_bottom, color_top, smoothstep(0.0, 1.0, val));

    gl_FragColor = color;
}