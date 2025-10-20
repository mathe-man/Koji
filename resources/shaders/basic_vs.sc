$input a_position, a_color0    // Input vars
$output v_color0               // Output to the fragment shader

#include "common.sh"            // Contain bgfx uniform definitions (e.g. u_modelViewProj)

void main()
{
    // Transform vertex position using the model-view-projection matrix
    gl_Position = mul(u_modelViewProj, vec(a_position, 1.0));
 
    // Pass the vertex color to the fragment shader    
    v_color0 = a_color0;
}

