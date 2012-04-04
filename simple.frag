/* FRAG */

uniform sampler2D TextureMap;

varying vec2 out_TexCoord;
varying vec3 out_Normal;

void main(void)
{
    vec4 tex0 = texture2D(TextureMap, out_TexCoord);
    gl_FragColor = vec4 (tex0.x, tex0.y, tex0.z, tex0.w);
    //gl_FragColor = vec4(1.0, 1.0, 0.0, 0.8);
}
