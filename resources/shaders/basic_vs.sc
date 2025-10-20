$input a_position, a_color0
$output v_color0

#include <bgfx_shader.sh>

void main()
{
    // Multiply the model matrix (set via bgfx::setTransform) with view-projection
    gl_Position = mul(u_model[0], vec4(a_position, 1.0));

    // Pass vertex color to fragment shader
    v_color0 = a_color0;
}

