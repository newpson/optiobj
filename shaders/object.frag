#version 110

const vec3 sun = vec3(0.0, 0.0, -1.0);
varying vec3 frag_normal;
uniform float u_is_selected;
uniform mat4 u_local_rotation;

void main()
{
    vec3 rotated_normal = (u_local_rotation * vec4(frag_normal, 1.0)).xyz;
    vec3 reflected = reflect(sun, rotated_normal);
    float intensity = clamp(1.0 - pow(dot(-sun, rotated_normal), 3.0), 0.1, 1.0);
    if (u_is_selected > 0.0)
        gl_FragColor = vec4(intensity, 1.0, 0.0, 1.0);
    else
        gl_FragColor = vec4(intensity, intensity, intensity, 1.0);
}
