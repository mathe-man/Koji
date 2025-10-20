$input v_color0     // Input from the vertex shader
$output o_color     // Output final color

void main()
{
    // Output the vertex color as the final fragment color
    o_color = v_color0;
}