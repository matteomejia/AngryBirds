#version 330 core
layout (points) in;
layout (triangle_strip, max_vertices = 18) out;

in VS_OUT {
    vec3 color;
} gs_in[];

out vec3 fColor;

void build_house(vec4 position)
{    
    fColor = gs_in[0].color; // gs_in[0] since there's only one input vertex

    gl_Position = position + vec4(0.2, 0.4, 0.0, 0.0); // 1 
    EmitVertex();

    gl_Position = position + vec4(0.4, 0.2, 0.0, 0.0); // 2
    EmitVertex();

    gl_Position = position + vec4(0.4, -0.2, 0.0, 0.0); // 3 
    EmitVertex();

    gl_Position = position + vec4(0.2, -0.4, 0.0, 0.0); // 4
    EmitVertex();

    gl_Position = position + vec4(-0.2, -0.4, 0.0, 0.0); // 5
    EmitVertex();

    gl_Position = position + vec4(-0.4, -0.2, 0.0, 0.0); // 6
    EmitVertex();

    gl_Position = position + vec4(-0.4, 0.2, 0.0, 0.0); // 7
    EmitVertex();

    gl_Position = position + vec4(-0.2,  0.4, 0.0, 0.0); // 8
    EmitVertex();

    gl_Position = position + vec4(0.2, 0.4, 0.0, 0.0); // 1 
    EmitVertex();

    gl_Position = position + vec4(0.4, -0.2, 0.0, 0.0); // 3 
    EmitVertex();

    gl_Position = position + vec4(-0.2, -0.4, 0.0, 0.0); // 5
    EmitVertex();

    gl_Position = position + vec4(-0.4, 0.2, 0.0, 0.0); // 7
    EmitVertex();

    gl_Position = position + vec4(0.4, 0.2, 0.0, 0.0); // 2
    EmitVertex();

    gl_Position = position + vec4(0.2, 0.4, 0.0, 0.0); // 1 
    EmitVertex();

    EndPrimitive();
}

void main() {    
    build_house(gl_in[0].gl_Position);
}