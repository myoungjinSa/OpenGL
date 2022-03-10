#version 400 core


in VS_OUT{
    vec4 COLOR;
    vec2 TEXCOORD;
} fs_in;

out vec4 out_color;

uniform sampler2D shaderTexture;

void main(){
    out_color = texture(shaderTexture, fs_in.TEXCOORD);
}