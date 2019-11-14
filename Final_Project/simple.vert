// //  Identity vertex shader
// varying vec3 Normal;
// varying vec4 Position;
// // uniform float blahhh;
//
// void main()
// {
//    //  Use color unchanged
//    Normal = gl_NormalMatrix * gl_Normal;
//
//    //  Set vertex coordinates
//    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
//    Position = gl_Position;
// }

varying float height;

//  Identity vertex shader
void main()
{
    /* Simple solid color for surface */
    // Use color unchanged
    // gl_FrontColor = gl_Color;

    // Set vertex coordinates
    // height will be the concentration value from the map at a time
    height = gl_Vertex.y;

    // Set constant y -- bc we just want color not a bump
    float new_y = 0.5; //gl_Vertex.y*5.0; // 0.05
    // gl_Vertex.y = -5;
    gl_Position = gl_ModelViewProjectionMatrix * vec4(gl_Vertex.x, new_y, gl_Vertex.z, 1);
}
