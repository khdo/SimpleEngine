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

uniform sampler2D texture0;
uniform sampler2D shadowmap[2];

varying vec3 normal;
varying vec4 pos;
varying vec4 shadowPos[2];
varying float size;

uniform mat4 irot;


float pcf(sampler2D t, vec4 p) {
    float s = 0;

    vec4 d = (p / p.w) - vec4(0, 0, 0.0005, 0);

    vec4 oioj = vec4(-0.002, -0.002, 0, 0);
    for (int i = 0; i < 4; i++) {
        oioj.y = -0.002;
        for (int j = 0; j < 4; j++) {
            if (d.z < texture2DProj(t, d + oioj).z)
                s += 1 / 16.0;
            oioj.y += 0.001;
        }
        oioj.x += 0.001;
    }
    return s;
}

vec4 getLightColor(int light) {
    vec4 lightColor;

    // Calculate light vector
    vec3 ldir = gl_LightSource[light].position.xyz - pos.xyz;
    // Distance from frag to light
    float d = length(ldir);
    // Attenuation
    float a = 1.0 / (gl_LightSource[light].constantAttenuation +
              (gl_LightSource[light].linearAttenuation * d) +
              (gl_LightSource[light].quadraticAttenuation * d * d));
    ldir = normalize(ldir);
    // Light source ambient contribution
    lightColor = gl_LightSource[light].ambient * gl_FrontMaterial.ambient;

    float spercent = pcf(shadowmap[light], shadowPos[light]);
    // Diffuse contribution
    float ndl = max(dot(normalize(normal), ldir), 0.0);
    vec4 dns = gl_LightSource[light].diffuse * gl_FrontMaterial.diffuse * ndl;

    // Specular contribution
    if (ndl > 0) {
        vec3 s = normalize(ldir + normalize(pos.xyz));
        dns += pow(max(dot(s, normalize(normal)), 0),
            gl_FrontMaterial.shininess) * gl_FrontMaterial.specular *
            gl_LightSource[light].specular;
    }
 
    lightColor += dns * spercent;

    // Attenuation
    lightColor *= a;
    return lightColor;
}

void main() {
    vec4 texColor0 = texture2D(texture0, gl_TexCoord[0].st);

    vec4 totalLightColor = getLightColor(0);
    totalLightColor += getLightColor(1);

    // Add in global ambiant
    totalLightColor += gl_FrontMaterial.emission + gl_FrontMaterial.ambient * gl_LightModel.ambient;

    gl_FragData[0] = texColor0 * totalLightColor;
    gl_FragData[1] = vec4(normal, 1);
    gl_FragData[2] = vec4(size, size, size, 1);
    //gl_FragColor = vec4(spercent, spercent, spercent, 1);
    //gl_FragColor = textureCube(cubetexture, rldir);
}
