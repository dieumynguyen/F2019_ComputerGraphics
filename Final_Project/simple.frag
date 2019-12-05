// //  Set the fragment color
// varying vec3 Normal;
// varying vec4 Position;
// // uniform float blahhh;
//
// void main()
// {
//     // gl_FragColor = vec4(Normal*time % 200, 1);
//     gl_FragColor = Position;
// }

varying float height;

// Set the fragment color

void main()
{
   // gl_FragColor = gl_Color;
   float x = height;
   vec3 C = vec3(height, 0.4, 0);
   gl_FragColor = vec4(C, 1);
}
