/* VERT */

uniform mat4 ModelViewMatrix;
uniform mat4 ProjMatrix;

attribute vec3 in_Position;
attribute vec2 in_TexCoord;
attribute vec3 in_Normal;

// output for fragment shader
varying vec2 out_TexCoord;
varying vec3 out_Normal;

void main(void)
{
    mat4 MVPMatrix = ProjMatrix * ModelViewMatrix;
    gl_Position = MVPMatrix * vec4(in_Position.x, in_Position.y, in_Position.z, 1.0);

    out_TexCoord.x = in_TexCoord.x;
    out_TexCoord.y = in_TexCoord.y;

    out_Normal = (MVPMatrix * vec4(in_Normal.x, in_Normal.y, in_Normal.z, 0.0)).xyz;
    out_Normal = normalize(out_Normal);
}

