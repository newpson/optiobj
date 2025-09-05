#version 110

attribute vec3 a_position;
attribute vec3 a_normal;
// attribute vec2 a_texture_coords;

uniform mat4 u_mvp;
// uniform vec3 u_camera_direction;

varying vec3 frag_normal;
// varying vec3 frag_camera_direction;
varying vec2 frag_texture_coords;

void main()
{
    // frag_camera_direction = u_camera_direction;
    frag_normal = a_normal;
    // frag_texture_coords = a_texture_coords;
    gl_Position = u_mvp * vec4(a_position, 1.0);
}
