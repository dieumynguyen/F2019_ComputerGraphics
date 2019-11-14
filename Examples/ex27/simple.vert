//  Identity vertex shader
varying vec3 Normal;
varying vec4 Position;
uniform float blahhh;

void main()
{
   //  Use color unchanged
   Normal = gl_NormalMatrix * gl_Normal;

   //  Set vertex coordinates
   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
   Position = gl_Position;
}
