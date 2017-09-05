// Copyright (c) 2011 Kha Do
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#version 120
#extension GL_EXT_geometry_shader4 : enable

varying in vec3 innormal[];
varying in vec4 inpos[];
varying in vec4 inshadowPos0[];
varying in vec4 inshadowPos1[];

varying out vec3 normal;
varying out vec4 pos;
varying out vec4 shadowPos[2];
varying out float size;


void main() {
    vec3 a = (inpos[0].xyz / inpos[0].w) - (inpos[1].xyz / inpos[1].w);
    vec3 b = (inpos[0].xyz / inpos[0].w) - (inpos[2].xyz / inpos[2].w);

    float area = length(cross(a, b)) * .005;
    
    gl_Position = gl_PositionIn[0];
    gl_TexCoord[0] = gl_TexCoordIn[0][0];
    normal = innormal[0];
    pos = inpos[0];
    shadowPos[0] = inshadowPos0[0];
    shadowPos[1] = inshadowPos1[0];
    size = area;
    EmitVertex();

    gl_Position = gl_PositionIn[1];
    gl_TexCoord[0] = gl_TexCoordIn[1][0];
    normal = innormal[1];
    pos = inpos[1];
    shadowPos[0] = inshadowPos0[1];
    shadowPos[1] = inshadowPos1[1];
    size = area;
    EmitVertex();
    
    gl_Position = gl_PositionIn[2];
    gl_TexCoord[0] = gl_TexCoordIn[2][0];
    normal = innormal[2];
    pos = inpos[2];
    shadowPos[0] = inshadowPos0[2];
    shadowPos[1] = inshadowPos1[2];
    size = area;
    EmitVertex();
    
    EndPrimitive();
}
