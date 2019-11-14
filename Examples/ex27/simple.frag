//  Set the fragment color
varying vec3 Normal;
varying vec4 Position;
uniform float blahhh;

void main()
{
    //gl_FragColor = vec4(Normal*time % 200, 1);
    gl_FragColor = Position;
}
