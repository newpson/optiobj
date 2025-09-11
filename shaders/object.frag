#version 110

const vec3 sun = vec3(0.0, 0.0, -1.0);
// varying vec3 frag_camera_direction;
varying vec3 frag_normal;
uniform float u_is_selected;

void main()
{
    vec3 reflected = reflect(sun, frag_normal);
    float intensity = clamp(1.0 - pow(dot(-sun, frag_normal), 3.0), 0.1, 1.0);
    // float intensity = clamp(dot(-normalize(frag_camera_direction), normalize(frag_normal)), 0.1, 1.0);
    if (u_is_selected > 0.0)
        gl_FragColor = vec4(intensity, 1.0, 0.0, 1.0);
    else
        gl_FragColor = vec4(intensity, intensity, intensity, 1.0);
}
