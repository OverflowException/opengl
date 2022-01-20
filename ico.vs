#version 330 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;

out vec3 frag_pos;
out vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main() {
    mat4 vm = view * model;
    gl_Position = proj * vm * vec4(a_pos.x, a_pos.y, a_pos.z, 1.0);

    frag_pos = vec3(vm * vec4(a_pos, 1.0));
    normal = mat3(transpose(inverse(vm))) * a_normal;
}