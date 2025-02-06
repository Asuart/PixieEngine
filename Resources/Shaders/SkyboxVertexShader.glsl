#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 mProjection;
uniform mat4 mView;

out vec3 localPos;

void main() {
    localPos = aPos;

    mat4 rotView = mat4(mat3(mView)); // remove translation from the view matrix
    vec4 clipPos = mProjection * rotView * vec4(localPos, 1.0);

    gl_Position = clipPos.xyww;
}